#pragma once

#include "IModule.hpp"

class Parser : public IModule {
private:
	void parseStartLine(Conversation& conv);
	void handleHugeStart(Conversation& conv);
	void parseHeader(Conversation& conv);
	void parseBody(Conversation& conv);
	void parseBodyChunked(Conversation& conv);
	void parseTrailer(Conversation& conv);

public:
	void execute(Conversation& conv);
};
