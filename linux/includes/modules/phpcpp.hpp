#ifndef CPPMODULE_HPP_
# define CPPMODULE_HPP_

# include <iostream>
# include <memory>
# include "IModule.hpp"
# include "http.h"
# include "net.h"
# include "conf.h"
# include "net.h"

namespace zia::api {
  class     cppModule : public IModule {
  public:
    cppModule();
    cppModule(const cppModule &);
    cppModule &operator=(const cppModule &);
    ~cppModule();

    virtual bool  config(const Conf& conf);
    virtual bool  exec(HttpDuplex& http);

    Net::Raw    stringToRaw(std::string const &str);
    std::string rawToString(zia::api::Net::Raw const &r);
    std::vector<std::string> split(std::string const &str, std::string const &delimiters);
    bool execRequest(HttpDuplex& http, std::string &url, std::string &args);

    char    **getEnv(HttpDuplex& http, std::string const &url);
    std::string   removeHeaders(std::string const &);
    char    **getArgs(std::string &url, std::string &args);
    void    freeTab(char **tab);

    virtual unsigned int getPriority() const { return 1; }
  private:
    const std::string _version = "7.2.2";
    const std::string _env[24] = {
      "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",
      "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",
      "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",
      "HTTP_HOST", "HTTP_USER_AGENT", "PATH",
      "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",
      "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
      "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",
      "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",
      "SERVER_SIGNATURE","SERVER_SOFTWARE" };
  };
}

#endif /* end of include guard: CPPMODULE_HPP_ */
