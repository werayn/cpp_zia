#ifndef HTMLMANAGER_HPP_
# define HTMLMANAGER_HPP_

# include <sys/types.h>
# include <dirent.h>
# include "http.h"
# include "Utils.hpp"
# include "Constant.hpp"


namespace zia::api {
  class HttpInterpreter;

  class     HtmlManager {
  public:
    /**
    * List the directory [path] and convert into html
    * throws FileNotFound
    * \return : the string containing the html
    */
    static std::string    viewDirectory(std::string const &path, std::string uri);

    /**
    * Convert the error [status] with message [reason] and add return link to [host] into HTML
    * throws FileNotFound
    * \return : the string containing the html
    */
    static std::string    viewError(http::Status const &status, std::string const &reason, std::string const &host);
  };
} /* zia::api */

#endif /* end of include guard: HTMLMANAGER_HPP_ */
