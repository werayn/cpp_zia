#ifndef SSLMODULE_HPP_
# define SSLMODULE_HPP_

# include <string>
# include <iostream>
# include <memory>
# include <cstring>
# include <limits.h>
# include <unistd.h>
# include "conf.h"
# include "http.h"
# include "IModule.hpp"

# include "openssl/bio.h"
# include "openssl/ssl.h"
# include "openssl/err.h"

typedef zia::api::Module *(*myModule)();

#define BUFSIZE 4096

using Ssl_ptr = std::unique_ptr<SSL, decltype(&SSL_free)>;
using Bio_ptr = std::unique_ptr<BIO, decltype(&BIO_free_all)>;
using Ctx_ptr = std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)>;

namespace zia::api {
  class SSLModule : public IModule {
  public:
    SSLModule();
    SSLModule(SSLModule const &);
    SSLModule &operator=(SSLModule const &);
    virtual ~SSLModule();

    virtual bool config(const Conf &conf);
    virtual bool exec(HttpDuplex &http);
    virtual unsigned int getPriority() const { return 0; }

  private:
    Bio_ptr       _sbio{NULL, BIO_free_all};
    Bio_ptr       _io{NULL, BIO_free_all};
    Bio_ptr       _sslbio{NULL, BIO_free_all};
    Ctx_ptr       _ctx{NULL, SSL_CTX_free};
    Ssl_ptr       _ssl{NULL, SSL_free};
    std::string   _cert;
    std::string   _key;
    std::string   _path;

    bool      initCtx();
    bool      loadCertificate();
    std::string getCurrentDir();
    template<typename E>
    bool      exitOnError(E e);
    std::string   rawToString(zia::api::Net::Raw const &r);
    zia::api::Net::Raw  stringToRaw(std::string const &str);
  };
}

#endif /* end of include guard: SSLMODULE_HPP_ */
