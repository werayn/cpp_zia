//
// daemon.cpp for zia in /home/koehle_j/rendu/cpp/cpp_zia
//
// Made by Jérémy Koehler
// Login   <koehle_j@epitech.net>
//
// Started on  Tue Jan  9 10:04:17 2018 Jérémy Koehler
// Last update Sun Feb 25 20:09:28 2018 Jérémy Koehler
//

#include "daemon.hpp"

zia::Daemon::~Daemon() {
  remove(_fileName.c_str());
  for (int x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
    close(x);
}

zia::Daemon		&zia::Daemon::getInstance(std::string file) {
  static zia::Daemon	daemon(file);

  return daemon;
}

void	zia::Daemon::sendSignal(std::string signal) {
  std::map<std::string, int> signals = {
    {"stop", SIGINT},
    {"quit", SIGQUIT},
    {"restart", SIGHUP},
    {"reload", SIGUSR1},
  };
  auto it = signals.find(signal);

  if (it != signals.end())
    {
      if (killProcess(it->second) == false)
	std::cerr << "Failed to send signal " << signal << std::endl;
    }
  else
    std::cerr << "Invalid signal " << signal << std::endl;
  if (signal == "restart")
    {
      int i = -1;

      std::cout << "Shutting down master process. It could take a while, please wait for a moment." << std::endl;
      while (fileExist("/run/zia.pid"))
	if (++i < 100)
	  break ;
      if (i == 100 && killProcess(SIGINT) == false) {
	std::cerr << "Failed to send signal " << signal << std::endl;
	return ;
      }
      while (fileExist("/run/zia.pid"));
      usleep(100);
    }
}

void	zia::Daemon::stop() {
  _killed = false;
  Logger::getInstance().info("[ZIA] shutting down");
}

bool	zia::Daemon::isAlive() {
  return _killed;
}

void	zia::Daemon::setConf(api::ConfigManager *conf) {
  _conf = conf;
}

zia::api::ConfigManager  *zia::Daemon::getConf() {
  return _conf;
}

void	zia::Daemon::setModuleManager(api::ModuleManager *m) {
  _m = m;
}

zia::api::ModuleManager  *zia::Daemon::getModuleManager() {
  return _m;
}

void	zia::Daemon::updateConf() {
  if (_conf)
    _conf->browser_conf();
}

bool	zia::Daemon::fileExist(std::string file) {
  struct stat info;
  return (stat(file.c_str(), &info) == 0);
}

zia::Daemon::Daemon(std::string file):
  _killed(true), _fileName(file), _conf(NULL) {
  signal(SIGCHLD, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  /* shutdown */
  signal(SIGTERM, zia::Daemon::quickShutdownSignal);
  signal(SIGINT, zia::Daemon::quickShutdownSignal);
  signal(SIGKILL, zia::Daemon::quickShutdownSignal);
  signal(SIGQUIT, zia::Daemon::gracefullShutdownSignal);

  /* restart */
  signal(SIGHUP, zia::Daemon::gracefullShutdownSignal);

  /* relaod conf */
  signal(SIGUSR1, zia::Daemon::reloadSignal);
  signal(SIGUSR2, zia::Daemon::reloadSignal);

  if (!fileExist(_fileName))
    {
      std::ofstream test(_fileName);

      if (!test.is_open()) {
	std::cerr << "Can not open file " << file << ": Permission denied" << std::endl;
	exit(2);
      };
      test.close();
      std::cout << "Starting a new session" << std::endl;
      daemonize();
    }
  else
    {
      std::cerr << "Can not launch server: " << _fileName << " already exist." << std::endl;
      exit(2);
    }
}

void	zia::Daemon::daemonize() {
  /* Fork off the parent process */
  zia::Daemon::closeParent();
  writePid();

  /* Close all open file descriptors */
  for (int x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
    close(x);
  Logger::getInstance().info("[ZIA] starting server");
}

void	zia::Daemon::closeParent() {
  pid_t	pid = fork();

  if (pid < 0)
    std::exit(EXIT_FAILURE);
  else if (pid > 0)
    std::exit(EXIT_SUCCESS);
  /* On success: The child process becomes session leader */
  if (setsid() < 0)
    std::exit(EXIT_FAILURE);
}

void		zia::Daemon::writePid() {
  std::ofstream	file;

  file.open("/run/zia.pid");
  file << getpid() << std::endl;
  file.close();
}

bool		zia::Daemon::killProcess(int signal) {
  pid_t		pid;
  std::ifstream	file;

  file.open("/run/zia.pid");
  if (file.is_open()) {
    file >> pid;
    file.close();
    kill(pid, signal);
    return true;
  }
  return false;
}

  // signal handlers
void	zia::Daemon::quickShutdownSignal(__attribute__((unused))int sig) {
  zia::Logger::getInstance().info("Quick shut down");
  zia::Daemon::getInstance().~Daemon();
  std::exit(0);
}

void	zia::Daemon::gracefullShutdownSignal(__attribute__((unused))int sig) {
  zia::Daemon::getInstance().stop();
}


void	zia::Daemon::reloadSignal(__attribute__((unused))int sig) {
  zia::Daemon::getInstance().updateConf();
  zia::Logger::getInstance().info("[ZIA] reload conf");
}
