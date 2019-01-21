//
// Balancer.cpp for  in /home/albert_q/rendu/Tech3/Zia/cpp_zia/src/Network
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Wed Feb  7 14:25:23 2018 Quentin Albertone
//

#include "Balancer.hpp"
#include "logger.hpp"

Balancer::Balancer()
  : _nbWorker(_DEBUG_NBWORKER)
{
  _daemon = &zia::Daemon::getInstance();
  createSocket();
  createWorker();
  acceptWorker();
  _daemon->getModuleManager()->getModules();
  _convert =  {
    { Network::SockType::SSL, "SSL" },
    { Network::SockType::PLAIN, "PLAIN" },
  };
}

Balancer::~Balancer()
{
}

void			Balancer::createSocket()
{
  t_sockaddr_un		addr;

  if ((_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 1)
    {
      _fd = -1;
      zia::Logger::getInstance().error("[BALANCER] - Failed to create unix socket");
      return ;
    }
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SRV_SOCK_PATH);
  if (access(SRV_SOCK_PATH, F_OK) == 0)
    unlink(addr.sun_path);
  if (bind(_fd, (t_sockaddr *)&addr, sizeof(t_sockaddr_un)) < 0)
    {
      // printf("Error bind on fd: %d\n", _fd);
      zia::Logger::getInstance().error("[BALANCER] Failed to bind af_unix socket");
      return ;
    }
  if (listen(_fd, SRV_BACKLOG))
    {
      zia::Logger::getInstance().error("[BALANCER] Failed to listen");
      // write(2, "Error listen\n", 13);
      return ;
    }
  zia::Logger::getInstance().info("[BALANCER] socket af_unix successfully created !");
}

int			Balancer::createWorker()
{
  int			i;
  int			pid;

  i = -1;
  zia::Logger::getInstance().debug("[BALANCER] - Create Worker begining - _nbWorker:" + std::to_string(_nbWorker));
  while (++i < _nbWorker)
    {
      if ((pid = fork()) == -1)
	{
	  zia::Logger::getInstance().error("[BALANCER] - Create Worker: Error fork");
	  return (1);
	}
      else if (pid == 0)
	{
	  zia::Logger::getInstance().info("[BALANCER] - Create Worker: child process");
	  Worker slave(i, _daemon);
	  slave.loop();
	  exit(1);
	}
      else
	zia::Logger::getInstance().info("[BALANCER] - Create Worker " + std::to_string(i) + " pid : " + std::to_string(pid));
      _worker.insert(std::pair<int, int>(i, i));
      usleep(100);
    }
  return (0);
}

void			Balancer::acceptWorker()
{
  int			i;

  i = -1;
  while (++i < _nbWorker)
    if ((_worker[i] = accept(_fd, NULL, NULL)) < 3)
      zia::Logger::getInstance().info("[BALANCER] - Accept Worker");
      //write(2, "Error: acceptWorker\n", 20);
}

// void			Balancer::display()
// {
//   std::map<int, int>::iterator it = _worker.begin();

//   zia::Logger::getInstance().error("[BALANCER] - Create Worker: Error fork");
//   printf("[Balancer:%d] -> fd: %d\n\tsrv_sock_path: %s - _debug_file: %s - _nbWorker: %d\n",
// 	 getpid(), _fd, SRV_SOCK_PATH, _DEBUG_FILE, _nbWorker);
//   for (; it != _worker.end(); ++it)
//     printf("\tWorker id: %d - fd: %d\n", it->first, it->second);
// }

int			Balancer::sendToWorker(int workerFd, std::pair<Client *, Network::SockType> client)
{
  struct msghdr		msg;
  char			buff[CMSG_SPACE(sizeof(client.first->getFd()))];
  std::string		tmp = std::string() + ((char *)((_convert[client.second]).c_str())) + "|" + std::to_string(client.first->getFd()).c_str();
  struct iovec  	iov = {.iov_base = ((char *)tmp.c_str()), .iov_len = 256};

  memset(&msg, 0, sizeof(struct msghdr));
  memset(buff, 0, sizeof(buff));
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = buff;
  msg.msg_controllen = sizeof(buff);

  struct cmsghdr	*cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  cmsg->cmsg_len = CMSG_LEN(sizeof(client.first->getFd()));

  *(int*) CMSG_DATA(cmsg) = client.first->getFd();

  msg.msg_controllen = cmsg->cmsg_len;

  if (sendmsg(workerFd, &msg, 0) < 0)
    {
      zia::Logger::getInstance().error("[BALANCER] - Failed communicate with worker: " + std::to_string(workerFd));
      return (-1); //printf("Failed communicate with worker:%2d\n", workerFd));
    }
  zia::Logger::getInstance().info("[BALANCER] - Send fd: " + std::to_string(client.first->getFd()) +
				  " of type " + std::string((char *)iov.iov_base) +
				  " to worker: " + std::to_string(workerFd));
  usleep(5);
  return (0);
}

int			Balancer::balancer(RequestList &req)
{
  int			err;
  std::pair<Client *, Network::SockType> client;

  err = 0;
  while (req.getSize() > 0)
    {
      client = req.popFrontReq();
      if (sendToWorker(_worker[client.first->getFd() % _nbWorker], client) != 0)
	err = -1;
    }
  return (err);
}
