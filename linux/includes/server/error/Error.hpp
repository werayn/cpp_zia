#ifndef ERROR_HPP_
# define ERROR_HPP_

# include <iostream>
# include <exception>

class                 FatalError : public std::exception {
public:
  FatalError(std::string const &from, std::string const &err) throw() {
    _err = "Fatal error (" + from + ") : " + err + ".";
  }
  virtual ~FatalError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }
private:
    std::string         _err;
};

class                 ModuleLoaderError : public std::exception {
public:
  ModuleLoaderError(std::string const &err) throw() {
    _err = "Module loader error : " + err + ".";
  }
  virtual ~ModuleLoaderError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

class                 ModuleNotFoundError : public std::exception {
public:
  ModuleNotFoundError(std::string const &name) throw() {
    _err = "Module " + name + " isn't loaded.";
  }
  virtual ~ModuleNotFoundError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

class                 ModuleManagerError : public std::exception {
public:
  ModuleManagerError(std::string const &err) throw() {
    _err = "Module manager error : " + err + ".";
  }
  virtual ~ModuleManagerError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

class                 TestError : public std::exception {
public:
  TestError(std::string const &err) throw() {
    _err = "Test error : " + err + ".";
  }
  virtual ~TestError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

class                 ConfigError : public std::exception {
public:
  ConfigError(std::string const &err) throw() {
    _err = "Config error : " + err + ".";
  }
  virtual ~ConfigError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

/**
* Thrown in case of invalid request
*/
class                 BadRequestError : public std::exception {
public:
  BadRequestError(std::string const &err) throw() {
    _err = "Bad request : " + err + ".";
  }
  virtual ~BadRequestError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

/**
* Thrown in case of invalid request
*/
class                 RequestUriTooLargeError : public std::exception {
public:
  RequestUriTooLargeError() throw() {}
  virtual ~RequestUriTooLargeError() throw() {}

  virtual const char  *what() const throw() { return ("Request Uri Too large"); }
};

/**
* Thrown in case of unsupported thing
*/
class                 NotImplementedError : public std::exception {
public:
  NotImplementedError(std::string const &err) throw() {
    _err = "Not implemented : " + err + ".";
  }
  virtual ~NotImplementedError() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

class                 FileNotFound : public std::exception {
public:
  FileNotFound(std::string const &err) throw() {
    _err = "File not found : " + err;
  }
  virtual ~FileNotFound() throw() {}

  virtual const char  *what() const throw() { return (_err.c_str()); }

  private:
      std::string         _err;
};

#endif /* !Error.hpp */
