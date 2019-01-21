//
// logger.cpp for zia in /home/koehle_j/rendu/cpp/cpp_zia
//
// Made by Jérémy Koehler
// Login   <koehle_j@epitech.net>
//
// Started on  Tue Feb 20 17:04:38 2018 Jérémy Koehler
// Last update Tue Feb 20 18:23:06 2018 Jérémy Koehler
//

#include "logger.hpp"

zia::Logger		&zia::Logger::getInstance()
{
  static zia::Logger	logger;

  return logger;
}

void			zia::Logger::debug(std::string message)
{
  writeLog("D", message);
}

void			zia::Logger::info(std::string message)
{
  writeLog("I", message);
}

void			zia::Logger::warning(std::string message)
{
  writeLog("W", message);
}

void			zia::Logger::error(std::string message)
{
  writeLog("E", message);
}

void			zia::Logger::writeLog(std::string type, std::string message)
{
  _file << getTime() << " " << type << " " << message << std::endl;
}

std::string		zia::Logger::getTime()
{
  std::time_t now;
  char buf[sizeof "YYYY-mm-DD HH:MM:SS"];

  time(&now);
  std::strftime(buf, sizeof buf, "%F %T", std::localtime(&now));
  return "[" + std::string(buf) + "]";
}

zia::Logger::Logger():
  _fileName("/var/log/zia/zia.log") {
  struct stat info;
  std::string	pathname = std::string("/var/log/zia");

  if (stat(pathname.c_str(), &info) != 0)
    {
      if (mkdir(pathname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0)
	{
	  std::cerr << "Can not access " << pathname << ": Permission denied" << std::endl;
	  exit(2);
	}
      else
	openLog();
    }
  else if (info.st_mode & S_IFDIR)
    openLog();
  else
    {
      std::cerr << pathname << " is not a directory" << std::endl;
      exit(2);
    }
}

void	zia::Logger::openLog()
{
  _file.open(_fileName, std::ofstream::out | std::ofstream::app);
  if (!_file.is_open()) {
    std::cerr << "Can not open file " << _fileName << ": Permission denied" << std::endl;
    exit(2);
  }
}
