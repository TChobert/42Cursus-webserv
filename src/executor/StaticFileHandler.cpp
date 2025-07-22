#include "StaticFileHandler.hpp"
#include <unistd.h>
#include <fcntl.h>

void	StaticFileHandler::handleStaticFile(Conversation& conv)
{
	const std::string& path = conv.req.pathOnDisk;

	int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd == -1)
	{
		conv.resp.status = INTERNAL_SERVER_ERROR;
		Executor::updateResponseData(conv);
		return;
	}

	conv.tempFd = fd;
	conv.state = READ_EXEC;
	conv.eState = READ_EXEC_GET_STATIC;
	return;
}
