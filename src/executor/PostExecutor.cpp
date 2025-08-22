#include "PostExecutor.hpp"
#include "Executor.hpp"
#include "MultipartParser.hpp"
#include "upload_utils.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <cstring>

//EXEMPLE REQUETE POST...

// POST /upload HTTP/1.1
// Host: example.com
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW
// Content-Length: 123456


//RFC 2046: le standard multipart dit que les frontieres sont precedees de 2 tirets (--) dans le body
//c'est la chaine qui va reellement apparaitre dans le body recu
static std::string extractBoundary(const std::string& contentType)
{
	std::string::size_type pos = contentType.find("boundary=");
	if (pos == std::string::npos)
		return "";

	return "--" + contentType.substr(pos + 9);
}

void PostExecutor::handlePost(Conversation& conv)
{
	conv.uploadedFiles.clear();
	conv.formFields.clear();

	if (CGIHandler::isCGI(conv))
	{
		CGIHandler::handleCGI(conv);
		return;
	}
	std::cout << "[handlePost] isCGI ? " << CGIHandler::isCGI(conv) << std::endl;

	std::string contentType = conv.req.header["content-type"];
	std::string boundary = extractBoundary(contentType);
	if (boundary.empty())
		Executor::sendErrorPage(conv, BAD_REQUEST);

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

		if (!conv.formFields.empty()) //si upload une simple image sans autre info, on ne cree pas un form
			saveFormSummary(conv);

		conv.req.pathOnDisk = "./www/upload_success.html";
		StaticFileHandler::handleStaticFile(conv, CREATED);
		// Executor::setResponse(conv, CREATED);
	}
	catch (std::exception& e)
	{
		Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
	}
}

// void PostExecutor::resumePostWriteBodyToCGI(Conversation& conv)
// {
// 	const size_t chunkSize = 4096;
// 	size_t toWrite = std::min(chunkSize, conv.req.body.size());

// 	ssize_t written = write(conv.writeFd, conv.req.body.c_str(), toWrite);

// 	std::cout << "[resumePostWrite] written: " << written << std::endl;

// 	if (written < 0)
// 	{
// 		std::cerr << "[ERROR resumePostWrite] write failed with errno=" << errno << " (" << strerror(errno) << ")" << std::endl;
// 		conv.fdsToClose.push_back(conv.writeFd);
// 		conv.writeFd = -1;
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 		return;
// 	}

// 	conv.req.body.erase(0, written);
// 	std::cout << "[resumePostWriteBodyToCGI] Remaining body size after erase: " << conv.req.body.size() << std::endl;

// 	if (conv.req.body.empty())
// 	{
// 		conv.fdsToClose.push_back(conv.writeFd);
// 		conv.writeFd = -1;
// 		conv.streamState = NORMAL;
// 		std::cout << "[resumePostWriteBodyToCGI] Finished writing body." << std::endl;
// 		conv.eState = READ_EXEC_POST_CGI;
// 		conv.state = READ_EXEC;
// 	}
// 	else
// 	{
// 		if (conv.headersSent)
// 			conv.streamState = STREAM_IN_PROGRESS;
// 		else
// 			conv.streamState = START_STREAM;
// 		conv.eState = READ_EXEC_POST_CGI;
// 		conv.state = READ_EXEC;
// 		std::cout << "[resumePostWriteBodyToCGI] More body to write. Staying in WRITE_EXEC_POST_CGI." << std::endl;
// 	}
// }

// void PostExecutor::resumePostReadCGI(Conversation& conv)
// {
// 	char buffer[4096];
// 	ssize_t bytesRead = read(conv.readFd, buffer, sizeof(buffer));

// 	std::cout << "[resumePostReadCGI] bytesRead: " << bytesRead << std::endl;

// 	if (bytesRead > 0)
// 	{
// 		conv.cgiOutput.append(buffer, bytesRead);
// 		std::cout << "[resumePostReadCGI] cgiOutput size: " << conv.cgiOutput.size() << std::endl;

// 		if (conv.streamState == START_STREAM)
// 		{
// 			CGIHandler::parseCgiOutput(conv);
// 			conv.streamState = STREAM_IN_PROGRESS;
// 		}
// 		else if (conv.streamState == STREAM_IN_PROGRESS)
// 			conv.resp.body.append(buffer, bytesRead);
// 		Executor::updateResponseData(conv);
// 		conv.state = RESPONSE;
// 	}
// 	else if (bytesRead == 0)
// 	{
// 		std::cout << "[resumePostReadCGI] EOF reached on fd " << conv.readFd << std::endl;
// 		conv.fdsToClose.push_back(conv.readFd);
// 		conv.readFd = -1;
// 		conv.cgiFinished = true;
// 		if (!hasCgiProcessExitedCleanly(conv.cgiPid))
// 		{
// 			std::cerr << "[resumePostReadCGI] CGI process did not exit cleanly." << std::endl;
// 			Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 			return;
// 		}

// 		if (conv.streamState == NORMAL)
// 			CGIHandler::parseCgiOutput(conv);
// 		Executor::updateResponseData(conv);
// 		conv.state = RESPONSE;
// 	}
// 	else
// 	{
// 		conv.fdsToClose.push_back(conv.readFd);
// 		conv.readFd = -1;
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 	}
// }




	// if (bytesRead > 0)
	// {
	// 	conv.cgiOutput.append(buffer, bytesRead);
	// 	std::cout << "[resumePostReadCGI] cgiOutput size: " << conv.cgiOutput.size() << std::endl;
	// }
	// else if (bytesRead == 0)
	// {
	// 	std::cout << "[resumePostReadCGI] EOF reached on fd " << conv.tempFd << std::endl;
	// 	conv.fdToClose = conv.tempFd;
	// 	conv.tempFd = -1;
	// 	if (!hasCgiProcessExitedCleanly(conv.cgiPid))
	// 	{
	// 		std::cerr << "[resumePostReadCGI] CGI process did not exit cleanly." << std::endl;
	// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	// 		return;
	// 	}
	// 	CGIHandler::parseCgiOutput(conv);
	// 	Executor::updateResponseData(conv);
	// 	conv.eState = EXEC_START;
	// 	conv.state = RESPONSE;
	// }
	// else
	// {
	// 	conv.fdToClose = conv.tempFd;
	// 	conv.tempFd = -1;
	// 	std::cerr << "[resumePostReadCGI] read error" << std::endl;
	// 	Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	// }
