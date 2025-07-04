#pragma once
#include <string>
#include <map>
#include <set>
#include "parse.hpp"

class Parser {
private:
	std::map<int,std::string>	buf;
	std::set<int>	expectsBody;

	void parseStartLine(Conversation& conv);
	void parseHeader(Conversation& conv);
	void parseBody(Conversation& conv);

public:
	void parse(Conversation& conv);
};
