//
// Worker.hpp for  in /home/albert_q/rendu/Tech3/Zia/cpp_zia/include/Network
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Tue Feb  6 11:03:59 2018 Quentin Albertone
// Last update Mon Feb 26 00:28:09 2018 Quentin Albertone
//

#ifndef WORKER_HPP_
# define WORKER_HPP_
# include "ziainclude.hpp"
# include "Network.hpp"
# include "http/HttpInterpreter.hpp"
# include "logger.hpp"
# include "Utils.hpp"


# ifndef __STREAMPROTO_
#  define __STREAMPROTO_

#  include <sys/un.h>
#  include <sys/socket.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>

#  include <sys/stat.h>
#  include <sys/time.h>

#  include <sys/wait.h>
# include <signal.h>

#  define SRV_SOCK_PATH		"/tmp/ux_socket"
#  define BUFFSIZE		1024
#  define SRV_BACKLOG		100

#  define _DEBUG_FILE		"/tmp/_debug"
#  define _DEBUG_NBWORKER	2

#  define _RIGHT		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
#  define _BEGINMSG		"[" << _pid << ":" << _id << "] - "

typedef struct sockaddr_un			t_sockaddr_un;
typedef struct sockaddr				t_sockaddr;
typedef std::pair<int, Network::SockType>	t_fd;
# endif /* !__STREAMPROTO_ */


class			Worker
{
public:
  Worker(int id, zia::Daemon *);
  ~Worker();

  void			receivFd();
  void			handleRequestFromClient();
  void			resetClient();

  void			createSocketWorker();
  void			loop();
  void			sendToClient(char *);
  void			historyClient(std::string , int);

  static void		signalHandler(int sig);

protected:
  int			_id;
  int			_srvFd;

  t_fd			_cliFd;
  std::string		_cliReq;

  int			_pid;
  int			_pPid;

  zia::Daemon		*_daemon;
  std::unique_ptr<zia::api::HttpInterpreter> _http{nullptr};
  std::map<std::string, Network::SockType>	_convert;
  std::map<int, int>				_hClient;
};


#endif /* !WORKER_HPP_ */
