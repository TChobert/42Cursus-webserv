#pragma once

#include <cstdlib>
#include "IModule.hpp"

class CgiExecutor : public IModule {

	private:

	int& _epollFd;

	public:

	CgiExecutor(int& epollFd);
	~CgiExecutor(void);
	void execute(Conversation& conv);
};
