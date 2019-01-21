//
// logger.hpp for zia in /home/koehle_j/rendu/cpp/cpp_zia
//
// Made by Jérémy Koehler
// Login   <koehle_j@epitech.net>
//
// Started on  Tue Feb 20 17:04:36 2018 Jérémy Koehler
// Last update Fri Feb 23 21:50:39 2018 Quentin Albertone
//

#ifndef LOGGER_HPP_
# define LOGGER_HPP_

#include <sys/types.h>
#include <sys/stat.h>

#include <ctime>
# include <iostream>
# include <fstream>

namespace zia {
  class Logger {
  public:
    static Logger	&getInstance();
    ~Logger()					= default;

    void	debug(std::string message);
    void	info(std::string message);
    void	warning(std::string message);
    void	error(std::string message);

  private:
    void	writeLog(std::string type, std::string message);
    std::string	getTime();

    Logger();
    void	openLog();

    std::string const	_fileName;
    std::ofstream	_file;

  public:
    //avoid copies
    Logger(Logger const&)			= delete;
    void	operator=(Logger const&)	= delete;
  };
}

#endif /* !LOGGER_HPP_ */
