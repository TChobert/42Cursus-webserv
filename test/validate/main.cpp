#include "../../inc/webserv.hpp"
#include <iostream>
#include <cassert>

using namespace std;

void test_validate_version() {
	Conversation conv;
	Validator v;
	/*
	conv.state = VALIDATE;
	conv.req.version = make_pair(1, 1);
	v.execute(conv);
	assert(conv.state == VALIDATE && "Valid version should pass");
	*/
	conv.req.version = make_pair(2, 0);
	v.execute(conv);
	assert(conv.resp.status == HTTP_VERSION_NOT_SUPPORTED && "Invalid HTTP version should fail");
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
	test_validate_version();
	cerr << "test_validate_version PASSED\n";
	test_validate_content_length();
	cerr << "test_validate_content_length PASSED\n";
	test_transfer_encoding();
	cerr << "test_transfer_encoding PASSED\n";
	test_uri_validation();
	cerr << "test_uri_validation PASSED\n";
	test_method_validation();
	cerr << "test_method_validation PASSED\n";
	test_host_header_validation();
	cerr << "test_host_header_validation PASSED\n";
}

//c++ -Wall -Wextra -std=c++98 -Iinc src/validate/*.cpp src/*.cpp  test/validate/main.cpp && ./a.out
