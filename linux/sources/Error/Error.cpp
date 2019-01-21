//
// Error.cpp for  in /home/albert_q/rendu/test/cpp/Error
//
// Made by Quentin Albertone
// Login   <albert_q@epitech.net>
//
// Started on  Tue Nov  7 15:40:00 2017 Quentin Albertone
// Last update Tue Nov  7 16:32:05 2017 Quentin Albertone
//

#include "Error.hpp"


// =============================================================== //

Error::TestError::TestError(std::string const &msg)
  : _msg(msg){
}

Error::TestError::TestError(TestError const &error)
  : _msg(error._msg){
}

Error::TestError::~TestError() throw() {
}

char const	*Error::TestError::what() const throw()
{
  return (_msg.c_str());
}

// =============================================================== //

Error::ThrowError::ThrowError(std::string const &msg)
  : TestError(msg){
}

Error::ThrowError::ThrowError(ThrowError const &other)
  : TestError(other){
}

Error::ThrowError::~ThrowError() throw(){
}
