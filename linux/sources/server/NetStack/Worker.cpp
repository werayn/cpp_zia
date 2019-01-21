//
// Worker.cpp for  in /home/albert_q/rendu/Tech3/Zia/cpp_zia/src/Network
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Tue Feb  6 11:03:49 2018 Quentin Albertone
// Last update Sat Mar  3 18:26:16 2018 Max
//

#include "Worker.hpp"

Worker::Worker(int id, zia::Daemon *daemon)
  : _id(id)
  , _daemon(daemon)
{
  signal(SIGTERM, Worker::signalHandler);
  signal(SIGINT, Worker::signalHandler);
  _pid = getpid();
  _pPid = getppid();

  zia::api::ConfigManager *conf = _daemon->getConf();
  zia::api::ModuleManager *manager = _daemon->getModuleManager();
  conf->getConf();
  manager->getModules();

  _http.reset(new zia::api::HttpInterpreter(conf->getConf(), conf->getRoots(), manager->getModules()));

  sleep(1);
  zia::Logger::getInstance().info("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Created end");

  createSocketWorker();
  _convert =  {
    { "SSL", Network::SockType::SSL},
    { "PLAIN", Network::SockType::PLAIN }
  };
}

Worker::~Worker()
{
  zia::Logger::getInstance().warning("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Worker is stopping his activity");
  close(_srvFd);
  for (int x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
    close(x);
}

void			Worker::createSocketWorker()
{
  t_sockaddr_un		addr;

  if ((_srvFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 3)
    {
      zia::Logger::getInstance().error("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Error create socket");
      return ;
    }
  zia::Logger::getInstance().info("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Create socket");
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SRV_SOCK_PATH);

  if (connect(_srvFd, (t_sockaddr *)&addr, sizeof(t_sockaddr_un)) < 0)
    {
      zia::Logger::getInstance().error("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Error connect Socket");
        return ;
    }
  zia::Logger::getInstance().info("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "] - Connect on " +
				  SRV_SOCK_PATH + std::to_string(_srvFd));
}

void			Worker::historyClient(std::string msg, int fd)
{
  int				netFd;
  std::vector<std::string>	tmp;

  tmp = Utils::split(msg, "|");
  netFd = std::stoi(tmp.back());
  _cliFd.second = _convert[tmp.front()];

  if (_hClient.find(netFd) == _hClient.cend())
    {
      _hClient.insert(std::pair<int, int>(netFd, fd));
      _cliFd.first = fd;
    }
  else
      _cliFd.first = _hClient[netFd];      
}

void			Worker::receivFd()
{
  unsigned char		*data;
  char			cBuffer[256];
  char			mBuffer[256] = {0};
  char			nBuffer[256];
  struct msghdr		msg;
  struct iovec		iov = {.iov_base = mBuffer, .iov_len = 256};
  struct cmsghdr	*cmsg;

  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_name = nBuffer;
  msg.msg_namelen = sizeof(nBuffer);
  msg.msg_control = cBuffer;
  msg.msg_controllen = sizeof(cBuffer);

  zia::Logger::getInstance().info("[WORKER] waiting for fd");
  if (recvmsg(_srvFd, &msg, 0) < 0)
    {
      zia::Logger::getInstance().error("[WORKER] failed to receive fd");
      return ;
    }
  cmsg = CMSG_FIRSTHDR(&msg);
  data = CMSG_DATA(cmsg);
  historyClient(std::string((char *)mBuffer), *((int *)data));
}

void			Worker::resetClient()
{
  _cliFd.first = 0;
  _cliReq.erase(_cliReq.begin(), _cliReq.end());
}

void			Worker::signalHandler(__attribute__((unused))int sig)
{
  exit(0);
}

void			Worker::sendToClient(char *buff)
{
  int                           len;
  int                           lenw;
  int                           err;

  lenw = 0;
  err = 0;
  if ((len = strlen(buff)) <= 0)
    return ;
  while (lenw != len)
    {
      len = len-lenw;
      lenw = send(_cliFd.first, buff + lenw, len, MSG_NOSIGNAL);
      if (++err == 10 || lenw < 0)
	{
	  zia::Logger::getInstance().error("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "]:"
					   + std::to_string(_cliFd.first) + "- Can't send answer to client");
	  free(buff);
	  return ;
	}
    }
  zia::Logger::getInstance().error("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "]:"
				   + std::to_string(_cliFd.first) + "- " + buff);
  free(buff);
}

void			Worker::handleRequestFromClient()
{
  std::vector<char>	buff(BUFFSIZE);
  int			bytes;

  if (_cliFd.second != Network::SockType::PLAIN)
    {
      _cliReq.erase(_cliReq.begin(), _cliReq.end());
      _cliReq = std::string("");
      return ;
    }
  while ((bytes = recv(_cliFd.first, &buff[0], BUFFSIZE, 0)) == BUFFSIZE)
    _cliReq.append(buff.cbegin(), buff.cend());
  _cliReq.append(buff.cbegin(), buff.cend());
  zia::Logger::getInstance().error("[" + std::to_string(_pid) + ":" + std::to_string(_id) + "]:"
				   + std::to_string(_cliFd.first) + "- " + _cliReq);
}

void			Worker::loop()
{
  std::string		resp;

    while (_daemon->isAlive())
    {
      receivFd();
      if (_cliFd.first > 0)
      	{
	  zia::Logger::getInstance().info("[" + std::to_string(_pid) + ":"
					  + std::to_string(_id) + "] - New client "
					  + std::to_string(_cliFd.first) + " of type "
					  + ((_cliFd.second == Network::SockType::SSL) ? "SSL" : "PLAIN"));
      	  handleRequestFromClient();
      	  resp = _http.get()->interpret(_cliReq, _cliFd.first);
	  if (_cliFd.second == Network::SockType::PLAIN)
	    {
	      zia::Logger::getInstance().debug("DEBUG: send to fd: " +  std::to_string(_cliFd.first));
	      if (send(_cliFd.first, resp.c_str(), resp.size(), 0) < 0)
		zia::Logger::getInstance().error("[" + std::to_string(_pid)
						 + ":" + std::to_string(_id) + "]:"
						 + std::to_string(_cliFd.first)
						 + " - Can't send answer to client");
	    }
	  resetClient();
	  resp.erase(resp.begin(), resp.end());
      	}
    }
}
