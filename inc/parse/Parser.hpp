#pragma once
#include <string>
class Conversation;

class Parser {
private:
	void parseStartLine(Conversation& conv);
	void handleHugeStart(Conversation& conv);
	void parseHeader(Conversation& conv);
	void parseBody(Conversation& conv);
	void parseBodyChunked(Conversation& conv);

public:
	void parse(Conversation& conv);
};
