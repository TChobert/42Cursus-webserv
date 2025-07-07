#pragma once
#include <string>
#include <map>
#include "parse/parse.hpp"

struct request {
	std::string method;
	std::string uri;
	std::pair<int,int> version;
	mapStr header;
	std::string body;
};

struct response {
	int statusCode;
	bool shouldClose;
};

enum statusCode {
	NOT_A_STATUS_CODE = 0,
	BAD_REQUEST = 400,
	URI_TOO_LONG = 414,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	NOT_IMPLEMENTED = 501,
};

enum convState {
	READ_CLIENT,
	WRITE_CLIENT,
	EOF_CLIENT,
	FINISH,
	READ_EXEC,
	WRITE_EXEC,
	PARSE_HEADER,
	PARSE_BODY,
	DRAIN_BUFFER,
	VALIDATE,
	RESPONSE,
	EXEC,
	IS_SENT
};

class Conversation {
public:
	int fd;
	request req;
	response resp;
	convState state;
	std::string buf;
	parseState pState;
	size_t bodyLeft;
	Conversation() : fd(-1), bodyLeft(0), state(DRAIN_BUFFER), pState(START) {};
};
