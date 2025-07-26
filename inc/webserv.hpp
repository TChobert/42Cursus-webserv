#pragma once

#include <string>
#include <vector>
#include <map>
#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include "parse/parse.hpp"
#include "read/Reader.hpp"
#include "validate/validate.hpp"
#include "serverConfig.hpp"
#include "execState.hpp"

class request {
public:
	std::string method;
	std::string uri;
	std::string pathOnDisk;
	std::string uploadFileName;
	bool hasQuery;
	std::string query;
	std::pair<int,int> version;
	mapStr header;
	size_t bodyLeft;
	std::string body;
};

struct response {
	statusCode status;
	bool shouldClose;
	std::string location;
	std::string contentType;
	std::vector<std::string> setCookies;
	mapStr header;
	std::string body;
};

class Conversation {
public:
	int fd;
	int tempFd;
	serverConfig config;
	locationConfig* location;
	request req;
	response resp;
	convState state;
	std::string buf;
	parseState pState;
	execState eState;
	std::string finalResponse;
	std::string cgiOutput;
	Conversation() : fd(-1), state(PARSE), pState(START), eState(EXEC_START) {};
};
