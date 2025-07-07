#pragma once
#include <string>
class Conversation;

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
