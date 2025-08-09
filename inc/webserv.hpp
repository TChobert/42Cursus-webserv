#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <csignal>
#include <unistd.h>

#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include "parse/parse.hpp"
#include "read/Reader.hpp"
#include "validate/validate.hpp"
#include "serverConfig.hpp"
#include "execState.hpp"
#include "keyValue.hpp"

#define MAX_SAFE_SIZE (10 * 1024 * 1024)
// #define MAX_FILE_LIMIT (50 * 1024 * 1024)

//volatile sig_atomic_t g_signal_status;

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

	response()
		: status(NOT_A_STATUS_CODE),
		  shouldClose(false),
		  location(""),
		  contentType(""),
		  setCookies(),
		  header(),
		  body("")
	{}
};

class Conversation {
public:
	int fd;
	int tempFd;
	int bodyFd;
	int fdToClose;
	bool cgiFinished;
	size_t bytesSent;
	pid_t cgiPid;
	struct timeval lastActive;
	time_t cgiStartTime;
	serverConfig config;
	locationConfig* location;
	request req;
	response resp;
	convState state;
	std::string buf;
	parseState pState;
	execState eState;
	streamState streamState;
	std::string finalResponse;
	std::string cgiOutput;
	std::map<std::string, std::string> formFields;
	std::vector<std::string> uploadedFiles;
	Conversation() : fd(-1), tempFd(-1), bodyFd(-1), fdToClose(-1), cgiFinished(false), bytesSent(0), cgiPid(-1), cgiStartTime(0), state(PARSE), pState(START), eState(EXEC_START), streamState(NORMAL) {};
};
