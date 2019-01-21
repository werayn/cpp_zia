#include "http/HttpParser.hpp"

namespace zia::api {
  HttpParser::HttpParser() : _logger(zia::Logger::getInstance()) {}

  HttpParser::HttpParser(Conf &conf) : _conf(conf), _logger(zia::Logger::getInstance()) {}

  HttpParser::HttpParser(HttpParser const &original) : _logger(zia::Logger::getInstance()) {
    _conf = original._conf;
  }

  HttpParser  &HttpParser::operator=(HttpParser const &original) {
    _conf = original._conf;
    return *this;
  }

  HttpParser::~HttpParser() {}

  struct HttpRequest          HttpParser::parse(std::string const &str) {
    std::vector<std::string>  v;
    std::vector<std::string>  line;
    struct HttpRequest        request;

    if (str.compare("") == 0)
      throw BadRequestError("empty request");
    v = Utils::split(str, "\n");
    line = Utils::split(inspectHttpLine(v[0]), " ");
    if (line.size() != 3)
      throw BadRequestError("invalid command line");
    request.version = getVersion(line[2]);
    request.headers = getHeaders(v);
    request.body = getBody(v);
    request.method = getMethod(line[0]);
    request.uri = line[1];
    try {
      if (_conf.find("max_uri_size") != _conf.end())
        if (line[1].size() > static_cast<unsigned long long>(std::get<long long>(_conf["max_uri_size"].v)))
          throw RequestUriTooLargeError();
    } catch (std::bad_variant_access &) {
      _logger.error("HttpParser : invalid variant access");
    }
    return request;
  }

  std::string     HttpParser::parse(struct HttpResponse const &response) const {
    std::string   str = "";

    str += getVersion(response.version) + ' ' + std::to_string(response.status) + ' ' + response.reason + "\r\n";
    str += getHeaders(response.headers);
    str += "\r\n";
    str += getBody(response.body);
    return str;
  }

  http::Version   HttpParser::getVersion(std::string const &version) const {
    if (version.compare("HTTP/0.9") == 0)
      return http::Version::http_0_9;
    if (version.compare("HTTP/1.0") == 0)
      return http::Version::http_1_0;
    if (version.compare("HTTP/1.1") == 0)
      return http::Version::http_1_1;
    if (version.compare("HTTP/2.0") == 0)
      return http::Version::http_2_0;
    return http::Version::unknown;
  }

  std::string   HttpParser::getVersion(http::Version const &version) const {
    switch (version) {
      case http::Version::http_0_9:
        return "HTTP/0.9";
        break;
      case http::Version::http_1_0:
        return "HTTP/1.0";
        break;
      case http::Version::http_1_1:
        return "HTTP/1.1";
        break;
      case http::Version::http_2_0:
        return "HTTP/2.0";
        break;
      default:
        return "HTTP/1.1";
        break;
    }
  }

  std::map<std::string, std::string>    HttpParser::getHeaders(std::vector<std::string> const &v) const {
    std::map<std::string, std::string>  m;
    std::vector<std::string>            line;
    unsigned int                        i = 1;

    while (i < v.size() && v[i].compare("\r") != 0) {
      line = Utils::split(inspectHttpLine(v[i]), ":");
      if (line.size() < 2)
        throw BadRequestError("invalid header (no key)");
      if (line[1][0] == ' ')
        line[1] = line[1].substr(1);
      m[line[0]] = "";
      for (size_t j = 1; j < line.size(); j++) {
        m[line[0]] += line[j];
        if ((j + 1) < line.size())
          m[line[0]] += ':';
      }
      i++;
    }
    return m;
  }

  std::string   HttpParser::getHeaders(std::map<std::string, std::string> const &headers) const {
    std::string str = "";

    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
      str += it->first + ": " + it->second + "\r\n";
    return str;
  }

  Net::Raw        HttpParser::getBody(std::vector<std::string> const &v) const {
    Net::Raw      body;
    unsigned int  i = 1;
    std::string   sbody = "";

    while (i < v.size() && v[i].compare("\r") != 0)
      i++;
    i++;
    if (i >= v.size())
      return body;
    while (i < v.size()) {
      sbody += v[i];
      i++;
      if (i < v.size())
        sbody += '\n';
    }
    sbody = sbody.substr(0, sbody.size());
    for (size_t j = 0; j < sbody.size(); j++)
      body.push_back(std::byte(sbody[j]));
    return body;
  }

  std::string   HttpParser::getBody(Net::Raw const &body) const {
    std::string str = "";

    for (Net::Raw::const_iterator it = body.begin(); it != body.end(); it++)
      str += (char)*it;
    return str;
  }

  http::Method  HttpParser::getMethod(std::string const &method) const {
    if (method.compare("OPTIONS") == 0)
      return http::Method::options;
    else if (method.compare("GET") == 0)
      return http::Method::get;
    else if (method.compare("HEAD") == 0)
      return http::Method::head;
    else if (method.compare("POST") == 0)
      return http::Method::post;
    else if (method.compare("PUT") == 0)
      return http::Method::put;
    else if (method.compare("DELETE") == 0)
      return http::Method::delete_;
    else if (method.compare("TRACE") == 0)
      return http::Method::trace;
    else if (method.compare("CONNECT") == 0)
      return http::Method::connect;
    return http::Method::unknown;
  }

  std::string   HttpParser::inspectHttpLine(std::string const &str) const {
    std::string nstr;

    if (Utils::trim(str) != str || str.size() < 1)
      throw BadRequestError("invalid http line");
    return str.substr(0, str.size() - 1);
  }

} /* zia::api */
