#include "Utils.hpp"

std::vector<std::string>    Utils::split(std::string const &str, std::string const &delimiters) {
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

std::string   Utils::trim(std::string const &str) {
  size_t      first = str.find_first_not_of(' ');
  size_t      last = str.find_last_not_of(' ');

  if (first == std::string::npos)
    return str;
  return str.substr(first, (last - first + 1));
}

std::string   Utils::readFile(std::string const &path) {
  if (access(path.c_str(), F_OK | R_OK) == -1)
    throw FileNotFound(path);
  std::ifstream file(path);
	std::string str((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  return str;
}

std::string   Utils::getExtension(std::string const &file) {
  std::string ext;
  size_t      pos;

  pos = file.find_last_of(".");
  if (pos == std::string::npos || pos == file.size() - 1)
    return "";
  return file.substr(pos + 1);
}

bool          Utils::isDirectory(std::string const &path) {
  struct stat buf;

  if (access(path.c_str(), F_OK | R_OK) == -1)
    throw FileNotFound(path);
  stat(path.c_str(), &buf);
  return S_ISDIR(buf.st_mode);
}

std::string   Utils::rawToString(zia::api::Net::Raw const &r) {
  std::string str;

  for (auto it = r.begin(); it != r.end(); it++)
    str += (char)*it;
  return str;
}

zia::api::Net::Raw      Utils::stringToRaw(std::string const &str) {
  zia::api::Net::Raw    r;

  for (auto it = str.begin(); it != str.end(); it++)
    r.push_back(std::byte(*it));
  return r;
}
