#pragma once
#include "parse.hpp"

const size_t startLineMax = 1e4;
const size_t methodMax = 8;
const size_t uriMax = 8000;
const size_t headerMax = 1e4;

class Parser {
private:
	void parseStartLine(Conversation& conv);
	void handleHugeStart(Conversation& conv);
	void parseHeader(Conversation& conv);
	void parseBody(Conversation& conv);

public:
	void parse(Conversation& conv);
};
