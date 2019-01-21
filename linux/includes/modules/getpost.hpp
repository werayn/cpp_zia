#ifndef __GETPOST_H__
# define __GETPOST_H__

#include <iostream>
#include <map>
#include <new>
#include <unistd.h>
#include <cstring>

std::string	urlDecode(std::string str);
void		initializeGet(std::map <std::string, std::string> &Get);
void		initializePost(std::map <std::string, std::string> &Post);
std::string	getpost_test();

#endif /*__GETPOST_H__*/
