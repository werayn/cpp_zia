//
// ziainclude.hpp for  in /home/albert_q/rendu/test/cpp/Socket
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Sun Nov  5 14:45:06 2017 Quentin Albertone
// Last update Sun Feb 25 22:24:42 2018 Quentin Albertone
//

#ifndef ZIAINCLUDE_HPP_
# define  ZIAINCLUDE_HPP_

// API
# include "../api/net.h"

// Common
# include <iostream>
# include <utility>
# include <vector>
# include <list>
# include <map>
# include <string>
# include <unistd.h>

// Socket
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netdb.h>

// poll
# include <poll.h>

# include  "daemon.hpp"

# define TCP			0

// Socket
typedef struct sockaddr_in	t_sockaddr_in;
typedef struct sockaddr		t_sockaddr;

// Poll
typedef struct pollfd		t_pollfd;

#endif /* !ZIAINCLUDE_HPP_ */
