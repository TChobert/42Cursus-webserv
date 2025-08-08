#include "PostExecutor.hpp"
#include "Executor.hpp"
#include "MultipartParser.hpp"
#include "upload_utils.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

void PostExecutor::handlePost(Conversation& conv)
{
	if (CGIHandler::isCGI(conv))
	{
		CGIHandler::handleCGI(conv);
		return;
	}

	std::string contentType = conv.req.header["content-type"];
	std::string::size_type pos = contentType.find("boundary=");
	if (pos == std::string::npos)
	{
		Executor::setResponse(conv, BAD_REQUEST);
		return;
	}

	std::string boundary = "--" + contentType.substr(pos + 9);

	try
	{
		MultipartParser parser(conv.req.body, boundary);
		std::vector<MultipartPart> parts = parser.parse();

		for (std::vector<MultipartPart>::size_type i = 0; i < parts.size(); ++i)
		{
			if (!parts[i].filename.empty() && !parts[i].data.empty())
				saveUploadedFile(parts[i], conv);
			else
				storeFormField(parts[i], conv);
		}

		saveFormSummary(conv);

		conv.resp.body = "<html><body><h1>Upload successful</h1></body></html>";
		Executor::setResponse(conv, CREATED);
	}
	catch (std::exception& e)
	{
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	}
}

// void PostExecutor::handlePost(Conversation& conv)
// {
// 	if (CGIHandler::isCGI(conv))
// 	{
// 		CGIHandler::handleCGI(conv);
// 		return;
// 	}
// 	if (!conv.location->uploadEnabled)
// 		return Executor::setResponse(conv, FORBIDDEN);
// 	if (conv.location->uploadStore.empty())
// 		return Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 	if (conv.req.uploadFileName.empty())
// 		conv.req.uploadFileName = generateUploadFileName();

// 	std::string uploadPath = conv.location->uploadStore + "/" + conv.req.uploadFileName;
// 	int fd = open(uploadPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
// 	if (fd < 0)
// 	{
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 		return;
// 	}

// 	conv.tempFd = fd;
// 	conv.eState = WRITE_EXEC_POST_BODY;
// 	conv.state = WRITE_EXEC;
// }

// void PostExecutor::resumePostWriteBodyToFile(Conversation& conv)
// {
// 	ssize_t written = write(conv.tempFd, conv.req.body.c_str(), conv.req.body.size());

// 	if (written < 0)
// 	{
// 		conv.fdToClose = conv.tempFd;
// 		conv.tempFd = -1;
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 		return;
// 	}

// 	//ne supprime que les octets qui ont ete ecrits
// 	conv.req.body.erase(0, written);

// 	if (conv.req.body.empty())
// 	{
// 		conv.fdToClose = conv.tempFd;
// 		conv.tempFd = -1;
// 		conv.resp.body = "<html><body><h1>Upload successful</h1></body></html>";
// 		Executor::setResponse(conv, CREATED);
// 	}
// 	else
// 	{
// 		//il reste des donnees a ecrire: on reste sur les memes states
// 		conv.eState = WRITE_EXEC_POST_BODY;
// 		conv.state = WRITE_EXEC;
// 	}
// }

void PostExecutor::resumePostWriteBodyToCGI(Conversation& conv)
{
	ssize_t written = write(conv.tempFd, conv.req.body.c_str(), conv.req.body.size());

	if (written < 0)
	{
		conv.fdToClose = conv.tempFd;
		conv.tempFd = -1;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	conv.req.body.erase(0, written);

	if (conv.req.body.empty())
	{
		conv.fdToClose = conv.tempFd;
		conv.tempFd = conv.bodyFd; //je switch et je stocke bodyFd dans tempFd (comme ca, il reste le seul a etre surveille par epoll)
		conv.bodyFd = -1;
		conv.eState = READ_EXEC_POST_CGI;
		conv.state = READ_EXEC;
	}
	else
	{
		conv.eState = WRITE_EXEC_POST_CGI;
		conv.state = WRITE_EXEC;
	}
}

void PostExecutor::resumePostReadCGI(Conversation& conv)
{
	char buffer[1024];
	ssize_t bytesRead = read(conv.tempFd, buffer, sizeof(buffer));

	if (bytesRead > 0)
		conv.cgiOutput.append(buffer, bytesRead);
	else if (bytesRead == 0)
	{
		conv.fdToClose = conv.tempFd;
		conv.tempFd = -1;
		if (!hasCgiProcessExitedCleanly(conv.cgiPid))
		{
			Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
			return;
		}
		CGIHandler::parseCgiOutput(conv);
		Executor::updateResponseData(conv);
		conv.eState = EXEC_START;
		conv.state = RESPONSE;
	}
	else
	{
		conv.fdToClose = conv.tempFd;
		conv.tempFd = -1;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	}
}
