#pragma once
#include <string>
#include <map>
#include "parse_utils.hpp"

struct request {
	std::string method;
	std::string uri;
	std::pair<int,int> version;
	mapStr header;
	std::string body;
};

class Conversation {
public:
	int fd;
	request req;
};

namespace ParserRoutine {
	std::string extractMethod(std::string& s);
	std::string extractRequestURI(std::string& s);
	std::pair<int, int> extractHTTPVersion(std::string& s);

	void parseHeader(Conversation& conv);
	pairStr extractOneHeader(std::string& s);

	std::string parseBodyUnchunked(std::string& s, int len);
	std::string parseBodyChunked(std::string& s);
};
