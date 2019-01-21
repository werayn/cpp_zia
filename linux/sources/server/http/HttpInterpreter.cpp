#include "http/HttpInterpreter.hpp"

namespace zia::api {
  HttpInterpreter::HttpInterpreter(Conf &conf, std::map<std::string, std::string> const &roots, ModulesList modules) : _conf(conf), _roots(roots), _modules(modules), _logger(zia::Logger::getInstance()) {
    _mimeType = {
      {"avi", "video/x-msvideo"},
      {"bin", "application/octet-stream"},
      {"css", "text/css"},
      {"csv", "text/csv"},
      {"doc", "application/msword"},
      {"gif", "image/gif"},
      {"htm", "text/html; charset=UTF-8"},
      {"html", "text/html; charset=UTF-8"},
      {"ico", "image/x-icon"},
      {"jar", "application/java-archive"},
      {"jpeg", "image/jpeg"},
      {"jpg", "image/jpeg"},
      {"js", "application/javascript"},
      {"json", "application/json"},
      {"mpeg", "video/mpeg"},
      {"otf", "font/otf"},
      {"png", "image/png"},
      {"pdf", "application/pdf"},
      {"rar", "application/x-rar-compressed"},
      {"rtf", "application/rtf"},
      {"sh", "application/x-sh"},
      {"svg", "image/svg+xml"},
      {"swf", "application/x-shockwave-flash"},
      {"tar", "application/x-tar"},
      {"wav", "audio/x-wav"},
      {"xhtml", "application/xhtml+xml"},
      {"xml", "application/xml"},
      {"zip", "application/zip"}
    };
    std::sort(_modules.begin(), _modules.end(), compareListItem);
  }

  HttpInterpreter::HttpInterpreter(HttpInterpreter const &original) : _logger(zia::Logger::getInstance()) {
    _conf = original._conf;
    _parser = original._parser;
    _roots = original._roots;
    _parser = original._parser;
    _modules = original._modules;
  }

  HttpInterpreter   &HttpInterpreter::operator=(HttpInterpreter const &original) {
    _conf = original._conf;
    _parser = original._parser;
    _roots = original._roots;
    _parser = original._parser;
    _modules = original._modules;
    return *this;
  }

  HttpInterpreter::~HttpInterpreter() {}

  std::string           HttpInterpreter::interpret(std::string const &request, size_t sock) {
    struct HttpDuplex   duplex;
    bool                ssl = (request.compare("") == 0);

    try {
      duplex.info.port = sock;
      duplex.raw_req = Utils::stringToRaw(request);
      if (ssl) {
        if (_modules.size() > 0 && _modules.front().priority == 0) {
          if (!_modules.front().module->exec(duplex))
            throw BadRequestError("SSL Module failed");
        } else
          throw BadRequestError("No SSL Module");
      }
      duplex.req = _parser.parse(Utils::rawToString(duplex.raw_req));
      duplex.resp = getDefaultResponse(duplex.req, http::common_status::ok, "OK");
      duplex.raw_req = Utils::stringToRaw(getRootFromHost(duplex.req.headers) + duplex.req.uri);
      for (auto it = _modules.begin(); it != _modules.end(); it++) {
        if (it->priority > 1)
          duplex.raw_req = Utils::stringToRaw(request);
        if (it->priority != 0 && !it->module->exec(duplex)) {
          _logger.error("Module method exec failed : " + it->name);
          duplex.resp = getDefaultResponse(duplex.req, http::common_status::internal_server_error, "Internal Server Error");
          break ;
        }
      }
      /* If no module has set a response, try to interpret the request */
      if (duplex.resp.body.empty()) {
        switch (duplex.req.method) {
          case http::Method::get:
            duplex.resp = get(duplex.req);
            break;
          case http::Method::head:
            duplex.resp = get(duplex.req, false);
            break;
          case http::Method::options:
          case http::Method::post:
          case http::Method::put:
          case http::Method::delete_:
          case http::Method::trace:
          case http::Method::connect:
            throw NotImplementedError("method isn't implemented");
            break;
          default:
            throw BadRequestError("invalid method");
            break;
        }
      }
    } catch (BadRequestError &e) {
      _logger.error(e.what());
      duplex.resp = getDefaultResponse(duplex.req, http::common_status::bad_request, "Bad Request");
    } catch (RequestUriTooLargeError &e) {
      _logger.error(e.what());
      duplex.resp = getDefaultResponse(duplex.req, http::common_status::request_uri_too_large, "Request-URI Too Long");
    } catch (NotImplementedError &e) {
      _logger.error(e.what());
      duplex.resp = getDefaultResponse(duplex.req, http::common_status::not_implemented, "Not Implemented");
    }
    if (duplex.resp.status != http::common_status::ok && duplex.resp.body.empty()) {
      try {
        duplex.resp.body = Utils::stringToRaw(HtmlManager::viewError(duplex.resp.status, duplex.resp.reason, duplex.req.headers["Host"]));
        duplex.resp.headers["Content-Type"] = _mimeType["html"];
        duplex.resp.headers["Content-Length"] = std::to_string(duplex.resp.body.size());
      } catch (std::exception &e) {
        _logger.error(e.what());
        duplex.resp.status = http::common_status::internal_server_error;
        duplex.resp.reason = "Internal Server Error";
      }
    }
    duplex.raw_resp = Utils::stringToRaw(_parser.parse(duplex.resp));
    /* If SSL Module, encode the response */
    if (ssl) {
      if (!_modules.front().module->exec(duplex))
        return _parser.parse(getDefaultResponse(duplex.req, http::common_status::internal_server_error, "Internal Server Error"));
      else
        return "";
    }
    _logger.info(Utils::rawToString(duplex.raw_resp));
    return Utils::rawToString(duplex.raw_resp);
  }

  struct HttpResponse   HttpInterpreter::getDefaultResponse(struct HttpRequest &req, http::Status const &status, std::string const &reason) {
    struct HttpResponse response;
    time_t              t = std::time(nullptr);

    response.version = http::Version::http_1_1;
    response.status = status;
    response.reason = reason;
    char buf[100];
    std::strftime(buf, 100, "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&t));
    response.headers["Date"] = buf;
    try {
      response.headers["Server"] = std::get<std::string>(_conf["server_name"].v) + "/" + std::get<std::string>(_conf["server_version"].v);
    } catch (std::bad_variant_access &) {
      _logger.error("HttpInterpreter : invalid variant access");
      response.headers["Server"] = "Zia/1.0";
    }
    if (req.headers.find("Cookie") != req.headers.end())
      response.headers["Cookie"] = req.headers["Cookie"];
    return response;
  }

  struct HttpResponse         HttpInterpreter::get(struct HttpRequest &request, bool body) {
    struct HttpResponse       response;
    std::vector<std::string>  v = Utils::split(request.uri, "?");
    HttpDuplex                duplex;

    try {
      std::string path = getRootFromHost(request.headers) + request.uri;
      response = getDefaultResponse(request, http::common_status::ok, "OK");
      if (_mimeType.find(Utils::getExtension(v[0])) != _mimeType.end()) {
        response.body = Utils::stringToRaw(Utils::readFile(path));
        response.headers["Content-Type"] = _mimeType[Utils::getExtension(v[0])];
        response.headers["Content-Length"] = std::to_string(response.body.size());
      } else if (Utils::isDirectory(path)) {
        response.body = Utils::stringToRaw(HtmlManager::viewDirectory(path, request.uri));
        response.headers["Content-Type"] = _mimeType["html"];
        response.headers["Content-Length"] = std::to_string(response.body.size());
      } else {
        response.body = Utils::stringToRaw(Utils::readFile(path));
        response.headers["Content-Type"] = "text/plain ; charset=UTF-8";
        response.headers["Content-Length"] = std::to_string(response.body.size());
      }
      if (!body)
        response.body.clear();
    } catch (FileNotFound &e) {
      _logger.error(e.what());
      return getDefaultResponse(request, http::common_status::not_found, "Not found");
    } catch (BadRequestError &e) {
      _logger.error(e.what());
      return getDefaultResponse(request, http::common_status::bad_request, "Bad Request");
    } catch (std::exception &e) {
      _logger.error(e.what());
      return getDefaultResponse(request, http::common_status::internal_server_error, "Internal Server Error");
    }
    return response;
  }

  std::string   HttpInterpreter::getRootFromHost(std::map<std::string, std::string> const &headers) {
    auto ith = headers.find("Host");

    if (ith == headers.end())
      throw BadRequestError("no Host header");
    std::vector<std::string> v = Utils::split(ith->second, ":");
    auto itr = _roots.find(v[0]);
    if (itr == _roots.end())
      throw BadRequestError("the specified host '" + ith->second + "' hasn't been found");
    return itr->second;
  }

  bool        HttpInterpreter::compareListItem(ListItem const &item1, ListItem const &item2) {
    return item1.priority < item2.priority;
  }
} /* zia:api */
