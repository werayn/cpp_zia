# pragma once

# include <iostream>
# include <string>
# include <fstream>
# include <iomanip>

# include "IModule.hpp"

# define FILE_PATH "Logs"

typedef zia::api::Module *(*myModule)();

namespace zia::api {
  class LogModule : public IModule {
    public:
      LogModule();
      LogModule(LogModule const &);
      LogModule &operator=(LogModule const &);
      virtual ~LogModule();

      virtual bool config(const Conf &conf);
      virtual bool exec(HttpDuplex &http);
      virtual unsigned int getPriority() const { return 100; };

    private:
      std::string _path;

      std::string printBody(Net::Raw const &body) const;
      std::string printVersion(http::Version v) const ;
      std::string printMethod(http::Method m) const;
      std::string   rawToString(zia::api::Net::Raw const &r);
  };
}
