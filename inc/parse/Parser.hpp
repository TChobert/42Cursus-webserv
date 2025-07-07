#pragma once
#include "parse.hpp"

const size_t startLineMax = 1e4;
const size_t methodMax = 8;
const size_t uriMax = 8000;
const size_t headerMax = 1e4;

class Parser {
private:
	bool parseStartLine(Conversation& conv);
	void handleHugeStart(Conversation& conv);
	bool parseHeader(Conversation& conv);
	bool parseBody(Conversation& conv);
	bool parseBodyChunked(Conversation& conv);

public:
	void parse(Conversation& conv);
};
