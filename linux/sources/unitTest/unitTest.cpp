#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "unitTest/unitTest.hpp"

using namespace zia::api;

TEST_CASE("Openssl certificate test", "[Openssl]") {
  std::string path = "linux/sources/unitTest/example/";

  std::unique_ptr<SSLModule> ssl = std::make_unique<SSLModule>();

  std::unique_ptr<ConfigManager> c = std::make_unique<ConfigManager>(path + "allright.json");
  c.get()->browser_conf();
  REQUIRE( ssl.get()->config(c.get()->getConf()) == true );

  c.reset(new ConfigManager(path + "NotTheGoodPath.json"));
  c.get()->browser_conf();
  REQUIRE( ssl.get()->config(c.get()->getConf()) == false );

  c.reset(new ConfigManager(path + "wrongCert.json"));
  c.get()->browser_conf();
  REQUIRE( ssl.get()->config(c.get()->getConf()) == false );

  c.reset(new ConfigManager(path + "wrongKey.json"));
  c.get()->browser_conf();
  REQUIRE( ssl.get()->config(c.get()->getConf()) == false );

  c.reset(new ConfigManager(path + "diffCertAndKey.json"));
  c.get()->browser_conf();
  REQUIRE( ssl.get()->config(c.get()->getConf()) == false );
}

TEST_CASE("Http parser test", "[HttpParser]") {
  HttpParser parser;
  std::string getEx = "GET / HTTP/1.1\r\nHost: localhost:80\r\nCookie: connect=true\r\n\r\n";
  struct HttpRequest req = parser.parse(getEx);
  REQUIRE(req.version == http::Version::http_1_1);
  REQUIRE(req.headers.find("Host") != req.headers.end());
  REQUIRE(req.headers.find("Cookie") != req.headers.end());
  REQUIRE(req.body.size() == 0);
  REQUIRE(req.method == http::Method::get);
  REQUIRE(req.uri.compare("/") == 0);

  std::string postEx = "POST /example/example.html HTTP/1.1\r\nHost: localhost:80\r\nCookie: tata=toto\r\nContent-Length: 23\r\nContent-Type: application/x-www-form-urlencoded\r\nConnexion: close\r\n\r\ntest1=toto&test2=foobar\n";
  req = parser.parse(postEx);
  REQUIRE(req.version == http::Version::http_1_1);
  REQUIRE(req.headers.find("Content-Length") != req.headers.end());
  REQUIRE(req.headers.find("Content-Type") != req.headers.end());
  REQUIRE(req.body.size() == std::stoi(req.headers["Content-Length"]));
  REQUIRE(req.method == http::Method::post);
  REQUIRE(req.uri.compare("/example/example.html") == 0);

  struct HttpResponse resp;
  resp.version = http::Version::http_1_1;
  resp.headers["Connexion"] = "close";
  resp.body = Utils::stringToRaw("hello toto");
  resp.status = http::common_status::ok;
  resp.reason = "OK";
  std::string expectedResp = "HTTP/1.1 200 OK\r\nConnexion: close\r\n\r\nhello toto";
  REQUIRE(parser.parse(resp).compare(expectedResp) == 0);
}
