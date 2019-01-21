#ifndef HTTPINTERPRETER_HPP_
# define HTTPINTERPRETER_HPP_

# include <iostream>
# include <string>
# include <ctime>
# include "http.h"
# include "Utils.hpp"
# include "module_manager/ModuleManager.hpp"
# include "Constant.hpp"
# include "http/HttpParser.hpp"
# include "http/HtmlManager.hpp"
# include "logger.hpp"

namespace zia::api {
  class   HttpInterpreter {
  public:
    /**
    * [conf] : the conf file
    * [roots] : the roots directory as ("host","root")
    * [modules] : the modules' list
    */
    HttpInterpreter(Conf &conf, std::map<std::string, std::string> const &root, ModulesList modules);
    HttpInterpreter(HttpInterpreter const &);
    HttpInterpreter   &operator=(HttpInterpreter const &);
    ~HttpInterpreter();

    /**
    * Interpret the request [request]
    * \return the formatted HTTP Response
    */
    std::string         interpret(std::string const &request, size_t sock);

    /**
    * Create a default HttpResponse with [status] and [reason] (has to be format with this._parser.parse(HttpResponse))
    * from the request [request]
    * \return the response
    */
    struct HttpResponse getDefaultResponse(struct HttpRequest &request, http::Status const &status = http::common_status::unknown, std::string const &reason = "");

    /**
    * Setters
    */
    void                setConf(Conf &conf) { _conf = conf; }
    void                setRoots(std::map<std::string, std::string> const &roots) { _roots = roots; }
    void                setModulesList(ModulesList modules) { _modules = modules; }

    /**
    * Getters
    */
    Conf                                getConf() const { return _conf; }
    std::map<std::string, std::string>  getRoots() const { return _roots; }
    ModulesList                         getModules() const { return _modules; }

  private:
    struct HttpResponse get(struct HttpRequest &request, bool body = true);

    std::string         getRootFromHost(std::map<std::string, std::string> const &);
    static bool         compareListItem(ListItem const &, ListItem const &);

    Conf                                _conf;
    HttpParser                          _parser{_conf};
    std::map<std::string, std::string>  _roots;
    std::map<std::string, std::string>  _mimeType;
    ModulesList                         _modules;
    zia::Logger                         &_logger;
  };
} /* zia::api */

#endif /* end of include guard: HTTPINTERPRETER_HPP_ */
