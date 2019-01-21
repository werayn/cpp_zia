//
// Request.hpp for  in /home/albert_q/rendu/test/cpp/Socket
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Wed Nov  8 17:10:18 2017 Quentin Albertone
// Last update Sun Feb 25 20:25:43 2018 Quentin Albertone
//

#ifndef REQUEST_HPP_
# define REQUEST_HPP_
# include "ziainclude.hpp"
# include "Client.hpp"

namespace Network {
  enum SockType
    {
      SSL,
      PLAIN
    };
}

class			RequestList
{
public:
  RequestList();
  RequestList(RequestList const &);
  ~RequestList();
  RequestList					&operator=(RequestList &);

  void						setRequest(Client *, Network::SockType);
  std::list<std::pair<Client *, Network::SockType>>				getRequestList();
  std::pair<Client *, Network::SockType>	popFrontReq();
  int						getSize();

  RequestList					&operator>>(Client *);
  RequestList					&operator+(RequestList const &);
  //  void						displayRequest();

protected:
  std::list<std::pair<Client *, Network::SockType>>	_request;
};

//bool						operator<(RequestList const &Request, RequestList const &list);
#endif /* ! REQUEST_HPP_ */
