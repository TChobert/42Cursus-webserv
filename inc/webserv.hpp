#pragma once
#include <string>
#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include "parse/parse.hpp"

class request {
public:
	std::string method;
	std::string uri;
	std::pair<int,int> version;
	mapStr header;
	size_t bodyLeft;
	std::string body;
};

struct response {
	statusCode status;
	bool shouldClose;
};

class Conversation {
public:
	int fd;
	request req;
	response resp;
	convState state;
	std::string buf;
	parseState pState;
	Conversation() : fd(-1), state(DRAIN_BUFFER), pState(START) {};
};
