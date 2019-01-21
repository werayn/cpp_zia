#include "SSLModule.hpp"

extern "C" {
  zia::api::SSLModule *create() {
    return (new zia::api::SSLModule);
  }
}

namespace zia::api {
  SSLModule::SSLModule() {}

  SSLModule::SSLModule(SSLModule const &copy) {
    (void)copy;
  }

  SSLModule &SSLModule::operator=(SSLModule const &copy) {
    (void)copy;
    return (*this);
  }

  SSLModule::~SSLModule() {}

  bool SSLModule::config(const Conf &conf) {
    Conf copy = conf;

    try {
      if (copy.find("SSL") == copy.end())
        throw std::logic_error("No SSL config");
      ConfObject obj = std::get<ConfObject>(copy["SSL"].v);
      if (obj.find("cert") == obj.end())
        throw std::logic_error("No certificate");
      _cert = std::get<std::string>(obj["cert"].v);
      if (obj.find("key") == obj.end())
        throw std::logic_error("No key");
      _key = std::get<std::string>(obj["key"].v);
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
    if (initCtx() == false)
      return false;
    return loadCertificate();
  }

  bool SSLModule::exec(HttpDuplex &http) {
    char          buf[BUFSIZE];
    std::string   req;

    _ssl.reset(SSL_new(_ctx.get()));
    SSL_set_fd(_ssl.get(), http.info.port);
    if (SSL_accept(_ssl.get()) <= 0)
      return exitOnError("SSL accept error");

    if (http.raw_resp.size() == 0) {
      while (!std::strcmp(buf,"\r\n") || !std::strcmp(buf,"\n")) {
        SSL_read(_ssl.get(), buf, sizeof(buf));
        req.append(buf);
      }
      http.raw_req = stringToRaw(req);
      std::cout << "Request == " << req << std::endl;
    } else {
      std::cout << "Response == " << rawToString(http.raw_resp) << std::endl;
      SSL_write(_ssl.get(), rawToString(http.raw_resp).c_str(),
                sizeof(rawToString(http.raw_resp).c_str()));
    }
    return true;
  }

  bool  SSLModule::initCtx() {
    const SSL_METHOD  *meth;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    meth = SSLv23_method();
    _ctx.reset(SSL_CTX_new(meth));
    if (!_ctx.get())
      return exitOnError("Cannot instantiate CTX");
    return true;
  }

  bool SSLModule::loadCertificate() {
    if (!(SSL_CTX_use_certificate_chain_file(_ctx.get(), _cert.c_str())))
      return exitOnError("Cannot use Certificate file: ");
    if (!(SSL_CTX_use_PrivateKey_file(_ctx.get(), _key.c_str(), SSL_FILETYPE_PEM)))
      return exitOnError("Cannot use PrivateKey fi, ERR_error_string( ERR_get_error(), NULL )le: ");
    if (!SSL_CTX_load_verify_locations(_ctx.get(), _cert.c_str(), NULL))
      return exitOnError("Cannot verify Certificate: ");
    return true;
  }

  std::string SSLModule::getCurrentDir() {
    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return std::string( result, (count > 0) ? count : 0 );
  }

  template<typename E>
  bool  SSLModule::exitOnError(E e) {
    std::cerr << e << ERR_error_string( ERR_get_error(), NULL ) << '\n';
    return false;
  }

  std::string SSLModule::rawToString(zia::api::Net::Raw const &r) {
    std::string str;

    for (auto it = r.begin(); it != r.end(); it++)
      str += (char)*it;
    return str;
  }

  zia::api::Net::Raw    SSLModule::stringToRaw(std::string const &str) {
    zia::api::Net::Raw  r;

    for (auto it = str.begin(); it != str.end(); it++)
      r.push_back(std::byte(*it));
    return r;
  }
}
