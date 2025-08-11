#include "CgiExecutor.hpp"

CgiExecutor::CgiExecutor(int& epollFd) : _epollFd(epollFd) {}

CgiExecutor::~CgiExecutor(void) {}

void CgiExecutor::execute(Conversation& conv) {

	const size_t chunkSize = 4096;
	
}
