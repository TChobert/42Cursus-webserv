#include "../../inc/webserv.hpp"
#include "serverConfig.hpp"
#include "webserv_enum.hpp"
#include <iostream>
#include <cassert>
#include <utility>

using namespace std;
locationConfig validLoc(string name) {locationConfig res; res.name = name; return res;}
Conversation validConv() {
	Conversation c;

	c.state = VALIDATE;
	c.req.method = "GET";
	c.req.uri = "/dir1/dir2/file1?query";
	c.req.version = make_pair(1,1);
	c.req.header["content-length"] = "5";
	c.req.header["host"] = "www.test.io";

	c.resp.status = NOT_A_STATUS_CODE;
	c.resp.shouldClose = false;

	c.config.identity.port = 8080;
	c.config.identity.host = "www.test.io";

	locationConfig a = validLoc("/");
	a.hasRoot = false;
	a.allowedMethods.push_back("GET");
	a.hasRedir = false;
	c.config.locations["/"] = a;

	locationConfig b = validLoc("/dir1");
	b.hasRoot = false;
	b.allowedMethods.push_back("GET");
	b.allowedMethods.push_back("POST");
	b.allowedMethods.push_back("DELETE");
	b.hasRedir = false;
	c.config.locations["/dir1"] = b;
	
	locationConfig rooted = validLoc("/dirRoot");
	rooted.hasRoot = true;
	rooted.root = "/another_dir";
	rooted.allowedMethods.push_back("POST");
	rooted.hasRedir = false;
	c.config.locations["/dirRoot"] = rooted;
	
	locationConfig reDir = validLoc("/dir3");
	reDir.hasRoot = false;
	reDir.allowedMethods.push_back("GET");
	reDir.hasRedir = true;
	reDir.redirCode = MOVED_PERMANENTLY;
	reDir.redirURL = "https://github.com";
	c.config.locations["/dir3"] = reDir;
	return c;
}
	

void test_happy_path() {
	Validator v;
	Conversation c = validConv();
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/dir1"]);
	assert(c.req.uri == "/dir1/dir2/file1");
	assert(c.req.hasQuery == true);
	assert(c.req.query == "query");
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == NOT_A_STATUS_CODE);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.uri = "http://www.test.io:8080/dir1/dir2/file1?query";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/dir1"]);
	assert(c.req.uri == "/dir1/dir2/file1");
	assert(c.req.hasQuery == true);
	assert(c.req.query == "query");
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == NOT_A_STATUS_CODE);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.header.erase("content-length");
	c.req.header["transfer-encoding"] = "chunked";
	c.req.uri = "/dir1a/banana.jpg";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/"]);
	assert(c.req.uri == "/dir1a/banana.jpg");
	assert(c.req.hasQuery == false);
	assert(c.resp.status == NOT_A_STATUS_CODE);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.uri = "/dirRoot/file";
	c.req.method = "POST";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/dirRoot"]);
	assert(c.req.uri == "/another_dir/file");
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == NOT_A_STATUS_CODE);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.uri = "/dir3/xXx_roXor_xXx";
	c.req.method = "GET";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/dir3"]);
	assert(c.req.uri == "https://github.com/xXx_roXor_xXx");
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == MOVED_PERMANENTLY);
	assert(c.resp.shouldClose == false);

}
	
void test_sad_path() {
	Validator v;

	Conversation c = validConv();
	c.req.version.second = 0;
	v.execute(c);
	assert(c.state == EXEC);
	assert(c.resp.status == HTTP_VERSION_NOT_SUPPORTED);
	assert(c.resp.shouldClose == true);

	c = validConv();
	c.req.header.erase("host");
	v.execute(c);
	assert(c.state == EXEC);
	assert(c.resp.status == BAD_REQUEST);
	assert(c.resp.shouldClose == true);

	c = validConv();
	c.req.header["host"] ="github.com";
	v.execute(c);
	assert(c.state == EXEC);
	assert(c.resp.status == BAD_REQUEST);
	assert(c.resp.shouldClose == true);

	c = validConv();
	c.req.header["transfer-encoding"] = "chunked";
	v.execute(c);
	assert(c.state == EXEC);
	assert(c.resp.status == BAD_REQUEST);
	assert(c.resp.shouldClose == true);

	c = validConv();
	c.req.uri = "/dirRoot/file";
	c.req.method = "GET";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == METHOD_NOT_ALLOWED);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.header["content-encoding"] = "gzip";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.resp.status == NOT_IMPLEMENTED);
	assert(c.resp.shouldClose == false);

	c = validConv();
	c.req.uri = "/dir1a/banana.jpg";
	c.config.locations.erase("/");
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.resp.status == NOT_FOUND);
	assert(c.resp.shouldClose == false);
}

int main() {
	test_happy_path();
	cerr << "test_happy_path PASSED\n";
	test_sad_path();
	cerr << "test_sad_path PASSED\n";
}

//c++ -Wall -Wextra -std=c++98 -Iinc src/validate/*.cpp src/*.cpp  test/validate/main.cpp && ./a.out
