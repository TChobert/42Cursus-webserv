#ifndef CONVERSATION_HPP
#define CONVERSATION_HPP

#include <string>
#include "HttpResponse.hpp"

enum State { OTHER, WRITE_CLIENT };

enum statusCode
{
	NOT_A_STATUS_CODE = 0,
	CONTINUE = 100,
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	TIMEOUT = 408,
	LENGTH_REQUIRED = 411,
	ENTITY_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501
};

struct Conversation {
    HttpResponse response;
    std::string final_response;
    int statusCode;
    State state;

    Conversation() : statusCode(200), state(OTHER) {}
};

#endif
