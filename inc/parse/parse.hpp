#pragma once
#include <string>
#include "parse_utils.hpp"

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

enum convState {
	READ_CLIENT,
	WRITE_CLIENT,
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

enum parseState {
	HEADER = 0,
	BODY,
	SKIP_BODY
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
};

namespace ParserRoutine {
	std::string extractMethod(std::string& s);
	std::string extractRequestUri(std::string& s);
	std::pair<int, int> extractHttpVersion(std::string& s);

	bool isValidFieldValue(std::string& s);
	mapStr parseAllField(std::string& s);
	pairStr extractOneField(std::string& s);

	std::string parseBodyUnchunked(std::string& s, int len);
	std::string parseBodyChunked(std::string& s);
};
