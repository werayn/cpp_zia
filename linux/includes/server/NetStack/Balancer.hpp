//
// Balancer.hpp for  in /home/albert_q/rendu/Tech3/Zia/cpp_zia/include/Network
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Wed Feb  7 14:26:37 2018 Quentin Albertone
// Last update Sun Feb 25 19:40:05 2018 Quentin Albertone
//

#ifndef BALANCER_HPP_
# define BALANCER_HPP_

# include "ziainclude.hpp"
# include "Worker.hpp"
# include "Request.hpp"

class			Balancer
{
public:
  Balancer();
  ~Balancer();

  void			createSocket();
  int			createWorker();
  void			acceptWorker();
  void			display();

  int			balancer(RequestList &req);
  int			sendToWorker(int workerFd, std::pair<Client *, Network::SockType>);

protected:
  int						_fd;
  std::map<int, int>				_worker;
  int						_nbWorker;

  zia::Daemon					*_daemon;
  std::map<Network::SockType, std::string>	_convert; // = {{Network::SockType::SSL}, {Network::SockType::PLAIN}};
};


#endif /* !BALANCER_HPP_ */
