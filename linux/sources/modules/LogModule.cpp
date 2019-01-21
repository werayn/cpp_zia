# include "LogModule.hpp"

extern "C" {
  zia::api::LogModule *create() {
    return (new zia::api::LogModule);
  }
}

namespace zia::api {
  LogModule::LogModule() : _path(FILE_PATH) {}

  LogModule::LogModule(LogModule const &copy) {
    (void)copy;
  }

  LogModule &LogModule::operator=(LogModule const &copy) {
    (void)copy;
    return (*this);
  }

  LogModule::~LogModule() {}

  bool LogModule::config(const Conf &conf) {
    Conf copy = conf;

    ConfObject obj = std::get<ConfObject>(copy["Log"].v);
    if (obj.find("path") != obj.end())
      _path = std::get<std::string>(obj["path"].v);
    return true;
  }

  bool LogModule::exec(HttpDuplex &http) {
    std::fstream file;
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    file.open(_path, std::fstream::in | std::fstream::out | std::fstream::app);
    file << "Time : " << std::put_time(std::localtime(&now_c), "%c") << '\n';
    file << "Request : " << rawToString(http.raw_req) << '\n';
    file.close();
    return true;
  }

  std::string LogModule::printBody(Net::Raw const &body) const {
    std::string str = "";

    for (auto it = body.begin(); it != body.end(); it++)
      str += (char)*it;
    return str;
  }

  std::string LogModule::printVersion(http::Version v) const {
    switch(v) {
      case http::Version::http_0_9 :
        return "http_0_9";
      case http::Version::http_1_0 :
        return "http_1_0";
      case http::Version::http_1_1 :
        return "http_1_1";
      case http::Version::http_2_0 :
        return "http_2_0";
      default :
        return "unknown";
    }
  }

  std::string LogModule::printMethod(http::Method m) const {
    switch(m) {
      case http::Method::options :
        return "OPTIONS";
      case http::Method::get :
        return "GET";
      case http::Method::head :
        return "HEAD";
      case http::Method::post :
        return "POST";
      case http::Method::put :
        return "PUT";
      case http::Method::delete_ :
        return "DELETE";
      case http::Method::trace :
        return "TRACE";
      case http::Method::connect :
        return "CONNECT";
      default:
        return "unknown";
    }
  }

  std::string LogModule::rawToString(zia::api::Net::Raw const &r) {
    std::string str;

    for (auto it = r.begin(); it != r.end(); it++)
      str += (char)*it;
    return str;
  }
}
