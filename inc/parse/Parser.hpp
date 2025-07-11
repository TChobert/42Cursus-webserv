#pragma once

#include "IModule.hpp"
#include <string>


class Parser : public IModule {
private:
	void parseStartLine(Conversation& conv);
	void handleHugeStart(Conversation& conv);
	void parseHeader(Conversation& conv);
	void parseBody(Conversation& conv);
	void parseBodyChunked(Conversation& conv);

public:
	void execute(Conversation& conv);
};
