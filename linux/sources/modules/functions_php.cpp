#include "getpost.hpp"
#include <sstream>

std::string     getpost_test() {
  std::map<std::string, std::string> Get;
  std::stringstream net;
  initializeGet(Get);
  net<< "<html><body>" << std::endl;
  net<< "<h1>Try post and get method</h1>" << std::endl;
  net<< "<form method=\"get\">" << std::endl;
  net<< " <label for=\"fname\">First name: </label>" << std::endl;
  net<< " <input type=\"text\" name=\"fname\" id=\"fname\"><br>" << std::endl;
  net<< " <label for=\"lname\">Last name: </label>" << std::endl;
  net<< " <input type=\"text\" name=\"lname\" id=\"lname\"><br>" << std::endl;
  net<< " <input type=\"submit\" />" << std::endl;
  net<< "</form><br /><br />" << std::endl;
  if (Get.find("fname")!=Get.end() && Get.find("lname")!=Get.end()) { 
    net << "Hello " << Get["fname"] << " " << Get["lname"]
              << ", I guess php-cgi module works" << std::endl;
  }
  else {
    net << "Fill up the above from and press submit" << std::endl;
  }
  net << "</body></html>" << std::endl;
  return (net.str());
}

std::string urlDecode(std::string str)
{
  std::string temp;
  char  tmp[5];
  char  tmpchar;
  int size = str.size();
  strcpy(tmp,"0x");
  for (int i = 0; i < size; i++) {
    if (str[i] == '%') {
      if (i + 2 < size) {
        tmp[2] = str[i + 1];
        tmp[3] = str[i + 2];
        tmp[4] = '\0';
        tmpchar = (char)strtol(tmp, NULL, 0);
        temp += tmpchar;
        i += 2;
        continue;
      }
      else {
        break;
      }
    }
    else if (str[i] == '+') {
      temp+=' ';
    }
    else {
      temp += str[i];
    }
  }
  return (temp);
}

void initializeGet(std::map <std::string, std::string> &Get)
{
  std::string tmpkey;
  std::string tmpvalue;
  std::string *tmpstr = &tmpkey;
  char* raw_get = getenv("QUERY_STRING");
  if (raw_get == NULL) {
    Get.clear();
    return;
 }
  while (*raw_get != '\0') {
    if (*raw_get == '&') {
      if (tmpkey != "") {
        Get[urlDecode(tmpkey)] = urlDecode(tmpvalue);
      }
      tmpkey.clear();
      tmpvalue.clear();
      tmpstr = &tmpkey;
    }
    else if (*raw_get=='=') {
      tmpstr = &tmpvalue;
    }
    else {
      (*tmpstr) += (*raw_get);
    }
    raw_get++;
  }
  if (tmpkey!="") {
    Get[urlDecode(tmpkey)] = urlDecode(tmpvalue);
    tmpkey.clear();
    tmpvalue.clear();
  }
}

void initializePost(std::map <std::string, std::string> &Post)
{
  std::string tmpkey;
  std::string tmpvalue;
  std::string *tmpstr = &tmpkey;
  int content_length;
  char *ibuffer;
  char *buffer = NULL;
  char *strlength = getenv("CONTENT_LENGTH");
  if (strlength == NULL) {
    Post.clear();
    return;
  }
  content_length = atoi(strlength);
  if (content_length == 0) {
    Post.clear();
    return;
  }
  try {
    buffer = new char[content_length*sizeof(char)];
  }
  catch (std::bad_alloc xa) {
    Post.clear();
    return;
  }
  if(fread(buffer, sizeof(char), content_length, stdin) != (unsigned int)content_length) {
    Post.clear();
    return;
  }
  *(buffer + content_length) = '\0';
  ibuffer = buffer;
  while (*ibuffer != '\0') {
    if (*ibuffer == '&') {
      if (tmpkey != "") {
        Post[urlDecode(tmpkey)] = urlDecode(tmpvalue);
      }
      tmpkey.clear();
      tmpvalue.clear();
      tmpstr = &tmpkey;
    }
    else if (*ibuffer == '=') {
      tmpstr = &tmpvalue;
    }
    else {
      (*tmpstr) += (*ibuffer);
    }
    ibuffer++;
  }
  if (tmpkey!="") {
    Post[urlDecode(tmpkey)] = urlDecode(tmpvalue);
    tmpkey.clear();
    tmpvalue.clear();
  }
}
