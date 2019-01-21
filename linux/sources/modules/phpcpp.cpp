#include "phpcpp.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>
#include "getpost.hpp"

extern "C" {
  zia::api::cppModule *create() {
    return (new zia::api::cppModule);
  }
}

zia::api::cppModule::cppModule() {}

zia::api::cppModule::cppModule(const zia::api::cppModule &copy) {
  (void)copy;
}

zia::api::cppModule &zia::api::cppModule::operator=(const zia::api::cppModule &copy) {
  (void)copy;
  return (*this);
}

zia::api::cppModule::~cppModule() {}

bool	zia::api::cppModule::config(const Conf& conf) {
  (void)conf;
  return (true);
}

std::string   zia::api::cppModule::rawToString(zia::api::Net::Raw const &r) {
  std::string str;

  for (auto it = r.begin(); it != r.end(); it++)
    str += (char)*it;
  return str;
}

zia::api::Net::Raw      zia::api::cppModule::stringToRaw(std::string const &str) {
  zia::api::Net::Raw    r;

  for (auto it = str.begin(); it != str.end(); it++)
    r.push_back(std::byte(*it));
  return r;
}

bool		zia::api::cppModule::exec(HttpDuplex& http) {
  std::vector<std::string> vGet = split(rawToString(http.raw_req), "?");
  std::string   url = vGet[0];
  std::string   args = "";

  if (http.req.method == http::Method::get) {
    args = (vGet.size() > 1 ? vGet[1] : "");
  }
  return (execRequest(http, url, args));
}

bool          zia::api::cppModule::execRequest(HttpDuplex& http, std::string &url, std::string &args) {
  int		link[2];
  int		linkIn[2];
  char		foo[4096 + 1];
  int		nbytes = 0;
  pid_t		pid;
  std::string	totalStr("");
  char **tab = getArgs(url, args);
  char **env = getEnv(http, url);

  if (split(url, ".").back().compare("php") == 0
    || split(url, ".").back().compare("html") == 0
    || split(url, ".").back().compare("htm") == 0) {
    memset(foo, 0, 4096);
    if (pipe(link) == -1)
      return false;
    if ((pid = fork()) == -1)
      return (false);
    if(pid == 0) {
      if (pipe(linkIn) == -1)
        return false;
      dup2(link[1], STDOUT_FILENO);
      dup2(linkIn[0], STDIN_FILENO);
      write(linkIn[1], rawToString(http.req.body).c_str(), http.req.body.size());
      close(link[0]);
      close(link[1]);
      close(linkIn[0]);
      close(linkIn[1]);
      execve("/usr/bin/php-cgi", tab, env);
      exit(0);
    } else {
      close(link[1]);
      freeTab(tab);
      freeTab(env);
      while(0 != (nbytes = read(link[0], foo, sizeof(foo)))) {
        foo[nbytes] = '\0';
      	totalStr = totalStr + foo;
      	memset(foo, 0, 4096);
      }
      http.resp.body = stringToRaw(removeHeaders(totalStr));
      http.resp.headers["Content-Type"] = "text/html; charset=UTF-8";
      http.resp.headers["Content-Length"] = std::to_string(http.resp.body.size());
      if (http.req.method == zia::api::http::Method::head)
        http.resp.body.clear();
      wait(NULL);
    }
    return (true);
  }
  return true;
}

std::string   zia::api::cppModule::removeHeaders(std::string const &str) {
  size_t      pos;

  pos = str.find("\r\n\r\n");
  if (pos == std::string::npos)
    return str;
  return str.substr(pos + 4);
}

char    **zia::api::cppModule::getEnv(HttpDuplex &http, std::string const &url) {
  char  **env = NULL;

  if (http.req.method == zia::api::http::Method::post && http.req.body.size() != 0
    && http.req.headers.find("Content-Type") != http.req.headers.end()
    && http.req.headers.find("Content-Length") != http.req.headers.end()) {
    env = new char*[6];
    env[0] = strdup("REQUEST_METHOD=POST");
    env[1] = strdup(std::string("CONTENT_LENGTH=" + http.req.headers["Content-Length"]).c_str());
    env[2] = strdup(std::string("CONTENT_TYPE=" + http.req.headers["Content-Type"]).c_str());
    env[3] = strdup("REDIRECT_STATUS=true");
    env[4] = strdup(std::string("SCRIPT_FILENAME=" + url).c_str());
    env[5] = NULL;
  }
  return env;
}

char                      **zia::api::cppModule::getArgs(std::string &url, std::string &args) {
  std::vector<std::string> v = split(args, "&");
  char      **argv = new char*[v.size() + 3];
  int       i = 2;

  argv[0] = strdup("-f");
  argv[1] = strdup(url.c_str());
  if (!v.empty()) {
    for (auto it = v.begin(); it != v.end(); ++it) {
      argv[i] = strdup(it->c_str());
      i++;
    }
  }
  argv[i] = NULL;
  return argv;
}

void        zia::api::cppModule::freeTab(char **tab) {
  int       i = 0;

  if (!tab)
    return ;
  while (tab[i]) {
    free(tab[i]);
    i++;
  }
  delete tab;
}

std::vector<std::string>    zia::api::cppModule::split(std::string const &str, std::string const &delimiters) {
  std::vector<std::string>  v;
  char                      *tmp;
  char                      *toFree = strdup(str.c_str());

  tmp = strtok(toFree, delimiters.c_str());
  while (tmp != NULL) {
    v.push_back(tmp);
    tmp = strtok(NULL, delimiters.c_str());
  }
  free(toFree);
  return v;
}
