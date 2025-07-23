#include "PostExecutor.hpp"
#include "Executor.hpp"
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

static std::string	generateUploadFileName()
{
	static int counter = 0;
	std::ostringstream ss;
	ss << "upload_" << counter++ << ".txt";
	return ss.str();
}

void PostExecutor::handlePost(Conversation& conv)
{
	if (CGIHandler::isCGI(conv))
	{
		CGIHandler::handleCGI(conv);
		return;
	}
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

	if (conv.req.uploadFileName.empty())
		conv.req.uploadFileName = generateUploadFileName();

	std::string uploadPath = conv.location->uploadStore + "/" + conv.req.uploadFileName;
	int fd = open(uploadPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	conv.tempFd = fd;
	conv.state = WRITE_EXEC;
	conv.eState = WRITE_EXEC_POST_BODY;
}

void PostExecutor::resumePostWriteBodyToFile(Conversation& conv)
{
	ssize_t written = write(conv.tempFd, conv.req.body.c_str(), conv.req.body.size());

	if (written < 0)
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	//ne supprime que les octets qui ont ete ecrits
	conv.req.body.erase(0, written);

	if (conv.req.body.empty())
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		conv.resp.body = "<html><body><h1>Upload successful</h1></body></html>";
		Executor::setResponse(conv, CREATED);
	}
	else
	{
		//il reste des donnees: on reste sur les memes states
		conv.state = WRITE_EXEC;
		conv.eState = WRITE_EXEC_POST_BODY;
	}
}

void PostExecutor::resumeReadPostCGI(Conversation&)
{

}

void PostExecutor::resumePostResponse(Conversation&)
{

}
