//
// main.cpp for  in /home/albert_q/rendu/test/cpp/Socket
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Sun Nov  5 16:42:43 2017 Quentin Albertone
//

#include "main.hpp"
#include "logger.hpp"

#include <getopt.h>
#include "daemon.hpp"
#include "http/HttpInterpreter.hpp"
#include "module_manager/ModuleManager.hpp"
#include "Network.hpp"

void		usage(std::string execName)
{
  std::cout << "Usage: " << execName << " [OPTION]" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -c, --config=file   set configuration file (default: /etc/zia/zia.json)" << std::endl;
  std::cout << "  -r, --reload        reload the configuration file" << std::endl;
  std::cout << "  -s, --signal=sig    send signal sig to the master process:" << std::endl;
  std::cout << "                        stop, quit, restart, reload" << std::endl;
  std::cout << "  -h, --help          display this help and exit" << std::endl;
}

int		process(std::string confPath)
{
  try {
    std::unique_ptr<zia::api::ConfigManager> p(new zia::api::ConfigManager(confPath));
    std::unique_ptr<zia::api::ModuleManager> m(new zia::api::ModuleManager);
    zia::Daemon &daemon = zia::Daemon::getInstance();

    if (!p.get()->browser_conf())
      zia::Logger::getInstance().error("Server launch only with his default value");

    Network::Socket	inet_http(std::get<long long>(p.get()->getConf()["port"].v), Network::PLAIN);
    Network::Socket	inet_ssl(std::get<long long>(p.get()->getConf()["port_ssl"].v), Network::SSL);

    std::map<std::string, std::string> map = p.get()->getRoots();
    m.get()->init(p.get()->getListModules(), p.get()->getConf());

    daemon.setConf(p.get());
    daemon.setModuleManager(m.get());

    Balancer		pipe;

    while (daemon.isAlive()) {
      inet_http.loop();
      inet_ssl.loop();
      pipe.balancer(inet_http.getRequest());
      pipe.balancer(inet_ssl.getRequest());
      sleep(3);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return (1);
  }
  return (0);
}

int		main(int argc, char **argv)
{
  static struct option long_options[] =
    {
      {"config", required_argument, 0, 'c'},
      {"reload", no_argument, 0, 'r'},
      {"signal", required_argument, 0, 's'},
      {"help", no_argument, 0, 'h'}
    };
  int option_index = 0;
  int c;
  std::string default_path = (zia::Daemon::fileExist("/etc/zia/zia.json")
			      ? "/etc/zia/zia.json"
			      : "./conf/zia.json");
  std::string confPath(default_path); // this will be the default value of our conf path

  while ((c = getopt_long (argc, argv, "c:rs:h",
			   long_options, &option_index)) != -1)
    {
      switch (c)
	{
	case 'c':
	  confPath = optarg;
	  break;
	case 'r':
	  zia::Daemon::sendSignal("reload");
	  return (0);
	case 's':
	  zia::Daemon::sendSignal(optarg);
	  if (std::string(optarg) != "restart")
	    return (0);
	  break;
	case 'h':
	  usage(argv[0]);
	  return (0);
	}
    }

  return (process(confPath));
}
