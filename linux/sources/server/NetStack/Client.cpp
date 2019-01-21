//
// Client.cpp for  in /home/albert_q/rendu/test/cpp/Socket
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Wed Nov  8 14:54:28 2017 Quentin Albertone
// Last update Sat Feb 24 20:31:59 2018 Quentin Albertone
//

#include "Client.hpp"

Client::Client(int server)
{
  zia::Logger::getInstance().info("[Balancer] - Acccepting new client");
  _size = sizeof(_sock);
  if ((_fd = accept(server, (t_sockaddr *)(&_sock), &_size)) < 3)
    zia::Logger::getInstance().error("[Balancer] - Client error while accepting new client");
  //  _addr = Sockets::GetAddress(_sock);
  _port = ntohs(_sock.sin_port);
  // Manage poll events POLLIN
  _poll.fd = _fd;
  _poll.events = POLLIN;

  zia::Logger::getInstance().info("[Balancer] - Client Accept new client");
  zia::Logger::getInstance().info("[Balancer] - Client new client: " + std::to_string(_fd) + " port: " +
				  _addr + std::to_string(_port));
}

Client::Client(Client const &client)
  : _fd(client._fd)
  , _sock(client._sock)
  , _size(client._size)
  , _port(client._port)
  , _addr(client._addr)
{
}

Client		&Client::operator=(Client const &client)
{
  if (this != &client)
    {
      _fd = client._fd;
      _sock = client._sock;
      _size = client._size;
      _port = client._port;
      _addr = client._addr;
    }
  return (*this);
}

Client::~Client()
{
  close(_fd);
}


// --------------------------------------------------------------------------------------------- //

Client			&Client::operator*()
{
  return (*this);
}

int			Client::getFd() const
{
  return (_fd);
}

t_sockaddr_in const	*Client::getSock()
{
  return (&_sock);
}

socklen_t		Client::getSize() const
{
  return (_size);
}

t_pollfd		Client::getPoll() const
{
  return (_poll);
}

std::string const	&Client::getAddr()
{
  return (_addr);
}

unsigned short		Client::getPort() const
{
  return (_port);
}
