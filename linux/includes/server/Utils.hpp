#ifndef UTILS_HPP_
# define UTILS_HPP_

# include <unistd.h>
# include <string>
# include <fstream>
# include <vector>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "Error.hpp"
# include "net.h"

class Utils {
public:
  /**
  * Split the string [str] at [delimiters]
  * \return a vector of the splited string
  */
  static std::vector<std::string> split(std::string const &str, std::string const &delimiters);

  /**
  * Remove spaces and tabs at the begin and at the end of the string [str]
  * \return the new string
  */
  static std::string  trim(std::string const &str);

  /**
  * Open and read the file [path]
  * throws FileNotFound
  * \return the opened file
  */
  static std::string  readFile(std::string const &path);

  /**
  * Get the extension of a file [file]
  * \return the extension or an empty string if there isn't
  */
  static std::string  getExtension(std::string const &file);

  /**
  * Check if [path] is a directory
  * throws FileNotFound
  * \return : bool
  */
  static bool         isDirectory(std::string const &path);

  /**
  * Transform a Net::Raw [r] into a string
  * \return : the string
  */
  static std::string  rawToString(zia::api::Net::Raw const &r);

  /**
  * Transform a string [str] into a Net::Raw
  * \return : the Net::Raw
  */
  static zia::api::Net::Raw stringToRaw(std::string const &str);
};

#endif /* end of include guard: UTILS_HPP_ */
