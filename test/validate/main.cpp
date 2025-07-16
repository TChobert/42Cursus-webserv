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
	c.config.locations["/dirRoot"] = reDir;
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
	c.req.header.erase("content-length");
	c.req.header["transfer-encoding"] = "chunked";
	c.req.uri = "/banana.jpg";
	v.execute(c);
	assert(c.state == PARSE_BODY);
	//assert(c.location == &c.config.locations["/"]);
	assert(c.req.uri == "/banana.jpg");
	assert(c.req.hasQuery == false);
	cerr << c.req.uri << endl;
	cerr << c.resp.status << endl;
	//assert(c.resp.status == NOT_A_STATUS_CODE);
	//assert(c.resp.shouldClose == false);
}
	
void test_sad_path() {
	Validator v;

	Conversation c = validConv();
	c.req.version.second = 0;
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
	v.execute(c);
	assert(c.state == PARSE_BODY);
	assert(c.location == &c.config.locations["/dir1"]);
	assert(c.req.uri == "/dir1/dir2/file1");
	assert(c.req.hasQuery == true);
	assert(c.req.query == "query");
	assert(c.req.bodyLeft == 5);
	assert(c.resp.status == NOT_A_STATUS_CODE);
	assert(c.resp.shouldClose == false);
}

void test_validate_content_length() {
	Conversation conv;
	Validator v;
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	conv.req.header["content-length"] = "10";
	v.execute(conv);
	assert(conv.req.bodyLeft == 10 && "Valid content-length should be accepted");

	conv.req.header["content-length"] = "abc";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == BAD_REQUEST && "Non-numeric content-length should be rejected");

	conv.req.header["content-length"] = "9999999999999999999999999";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == ENTITY_TOO_LARGE && "Too large content-length should be rejected");
}

void test_transfer_encoding() {
	Conversation conv;
	Validator v;
	/*
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	conv.req.header["transfer-encoding"] = "chunked";
	v.execute(conv);
	assert(conv.state == VALIDATE && "Chunked transfer encoding should be accepted");
	*/

	conv.req.header["transfer-encoding"] = "gzip";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == NOT_IMPLEMENTED && "Unsupported transfer-encoding should fail");
}

void test_uri_validation() {
	Conversation conv;
	Validator v;
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	conv.req.method = "GET";
	conv.req.uri = "/valid/path";
	conv.req.header["host"] = "localhost";
	conv.config.identity.host = "localhost";
	conv.config.identity.port = 8080;
	locationConfig loc;
	loc.allowedMethods.push_back("GET");
	conv.config.locations["/valid"] = loc;
	v.execute(conv);
	assert(conv.state == VALIDATE && "Valid URI should pass");

	conv.req.uri = "/invalid/%ZZpath";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == BAD_REQUEST && "Invalid percent encoding should be rejected");
}

void test_method_validation() {
	Conversation conv;
	Validator v;
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	conv.req.uri = "/";
	conv.req.header["host"] = "localhost";
	conv.config.identity.host = "localhost";
	conv.config.identity.port = 8080;

	locationConfig loc;
	loc.allowedMethods.push_back("GET");
	conv.config.locations["/"] = loc;
	conv.location = &conv.config.locations["/"];

	conv.req.method = "GET";
	v.execute(conv);
	assert(conv.state == VALIDATE && "GET should be allowed");

	conv.req.method = "POST";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == METHOD_NOT_ALLOWED && "Disallowed method should return 405");
}

void test_host_header_validation() {
	Conversation conv;
	Validator v;
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	conv.req.method = "GET";
	conv.req.uri = "/";
	conv.config.identity.host = "example.com";
	conv.config.identity.port = 8080;

	locationConfig loc;
	loc.allowedMethods.push_back("GET");
	conv.config.locations["/"] = loc;
	conv.location = &conv.config.locations["/"];

	conv.req.header["host"] = "example.com";
	v.execute(conv);
	assert(conv.state == VALIDATE && "Correct Host header should pass");

	conv.req.header["host"] = "invalid.com";
	conv.state = VALIDATE;
	v.execute(conv);
	assert(conv.resp.status == BAD_REQUEST && "Wrong Host header should fail");
}

int main() {
	test_happy_path();
	cerr << "test_happy_path PASSED\n";
}

//c++ -Wall -Wextra -std=c++98 -Iinc src/validate/*.cpp src/*.cpp  test/validate/main.cpp && ./a.out
