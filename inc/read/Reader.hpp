#pragma once
#include "../webserv.hpp"

class Reader {
private:
	const int BUFFER_SIZE = 1 << 20;

public:
	void read(Conversation& conv);
};

