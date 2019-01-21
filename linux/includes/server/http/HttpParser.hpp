#ifndef HTTPPARSER_HPP_
# define HTTPPARSER_HPP_

# include <string>
# include "http.h"
# include "Utils.hpp"
# include "Error.hpp"
# include "conf.h"
# include "logger.hpp"

namespace zia::api {
  class                 HttpParser {
  public:
    HttpParser();
    HttpParser(Conf &conf);
    HttpParser(HttpParser const &);
    HttpParser  &operator=(HttpParser const &);
    ~HttpParser();

    /**
    * Parse the string [input] to make a HttpRequest
    * Exception : BadRequestError, RequestUriTooLargeError
    * \return the parsed request
    */
    struct HttpRequest  parse(std::string const &input);

    /**
    * Parse the HttpResponse [response] to make a string
    * \return the formatted response
    */
    std::string         parse(struct HttpResponse const &response) const;

  private:
    http::Version                       getVersion(std::string const &) const;
    std::string                         getVersion(http::Version const &) const;
    std::map<std::string, std::string>  getHeaders(std::vector<std::string> const &) const;
    std::string                         getHeaders(std::map<std::string, std::string> const &) const;
    Net::Raw                            getBody(std::vector<std::string> const &) const;
    std::string                         getBody(Net::Raw const &) const;
    http::Method                        getMethod(std::string const &) const;
    std::string                         inspectHttpLine(std::string const &) const;

    Conf                                _conf;
    zia::Logger                         &_logger;
  };
} /* zia::api */

#endif /* end of include guard: HTTPPARSER_HPP_ */
