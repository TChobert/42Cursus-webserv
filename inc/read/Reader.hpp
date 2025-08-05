#pragma once
#include "../webserv.hpp"

class Reader : public IModule {
private:
	static const int BUFFER_SIZE = 1 << 16;
public:
	void execute(Conversation& conv);
};

