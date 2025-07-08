#pragma once
#include <string>
#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include "parse/parse.hpp"

struct request {
	std::string method;
	std::string uri;
	std::pair<int,int> version;
	mapStr header;
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
	size_t bodyLeft;
	Conversation() : fd(-1), bodyLeft(0), state(DRAIN_BUFFER), pState(START) {};
};
