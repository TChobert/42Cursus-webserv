#include "PostExecutor.hpp"
#include "Executor.hpp"
#include <unistd.h>
#include <fcntl.h>

void PostExecutor::handlePost(Conversation& conv)
{
	// if (CGIHandler::isCGI(conv))
	// {
	// 	CGIHandler::handleCGI(conv);
	// 	return;
	// }
	if (!conv.location->uploadEnabled)
	{
		Executor::setResponse(conv, FORBIDDEN);
		return;
	}
	if (conv.location->uploadStore.empty())
	{
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	//open fd for the filename

	//conv.tempFd = fd;
	conv.state = READ_EXEC;
	conv.eState = READ_EXEC_POST_BODY;
}

// void PostExecutor::resumePostBody(Conversation&)
// {

// }

// void PostExecutor::resumePostUpload(Conversation&)
// {

// }

// void PostExecutor::resumeReadPostCGI(Conversation&)
// {

// }

// void PostExecutor::resumePostResponse(Conversation&)
// {

// }
