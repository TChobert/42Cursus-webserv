#pragma once
#include <string>
#include <vector>
#include <map>
#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include "parse/parse.hpp"
#include "read/Reader.hpp"
#include "validate/validate.hpp"

struct locationConfig {
	std::string name;
	std::vector<std::string> allowedMethods;
	std::string index;
	bool autoIndex;
	bool hasReturn;
	int returnCode;
	std::string returnURL;
};

struct serverInfo {

	unsigned int	port;
	std::string		host;
	std::string		server_name;
	size_t clientMaxBodySize;
};

struct errorPages {

	std::string	error_400;
	std::string	error_403;
	std::string	error_404;
	std::string	error_405;
	std::string	error_408;
	std::string	error_413;
	std::string	error_500;
	std::string	error_501;
	std::string	error_504;
};

struct serverConfig {
	serverInfo		identity;
	errorPages		errorPagesPaths;
	std::map<int, std::string>	errorPagesCodes;
	std::map<std::string, locationConfig> locations;
};

class request {
public:
	std::string method;
	std::string uri;
	parsedPath path;
	std::pair<int,int> version;
	mapStr header;
	size_t bodyLeft;
	std::string body;
};

struct response {
	statusCode status;
	bool shouldClose;
	mapStr header;
	std::string body;
};

class Conversation {
public:
	int fd;
	serverConfig config;
	locationConfig* location;
	request req;
	response resp;
	convState state;
	std::string buf;
	parseState pState;
	Conversation() : fd(-1), state(PARSE), pState(START) {};
};
