//
// daemon.hpp for zia in /home/koehle_j/rendu/cpp/cpp_zia
//
// Made by Jérémy Koehler
// Login   <koehle_j@epitech.net>
//
// Started on  Tue Jan  9 10:06:13 2018 Jérémy Koehler
//

#ifndef DAEMON_HPP_
# define DAEMON_HPP_

# include <cstdlib>
# include <unistd.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <syslog.h>

# include <iostream>
# include <fstream>

# include "config_manager/ConfigManager.hpp"
# include "logger.hpp"

namespace zia {
  class Daemon {

  public:
    static Daemon	&getInstance(std::string file = "/run/zia.pid");
    ~Daemon();

    static void		sendSignal(std::string signal);

    void		stop();
    bool		isAlive();
    void		setConf(api::ConfigManager *conf);
    api::ConfigManager	*getConf();
    void		setModuleManager(api::ModuleManager *m);
    api::ModuleManager	*getModuleManager();
    void		updateConf();

    // signal handlers
    static void		quickShutdownSignal(int sig);
    static void		gracefullShutdownSignal(int sig);
    static void		reloadSignal(int sig);
    static bool		fileExist(std::string file);

  private:
    Daemon(std::string file);
    static void		daemonize();
    static void		closeParent();
    static void		writePid();
    static bool		killProcess(int signal);

    bool	_killed;
    std::string	_fileName;
    api::ConfigManager *_conf;
    api::ModuleManager *_m;

  public:
    //avoid copies
    Daemon(Daemon const&)			= delete;
    void	operator=(Daemon const&)	= delete;
  };
}

#endif /* !DAEMON_HPP_ */
