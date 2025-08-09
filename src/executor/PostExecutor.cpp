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

	std::string contentType = conv.req.header["content-type"];
	std::string boundary = extractBoundary(contentType);
	if (boundary.empty())
		return Executor::setResponse(conv, BAD_REQUEST);

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

		conv.resp.body = "<html><body>"
						"<h1>Upload successful</h1>"
						"<p><a href=\"/\">Back Welcome Page</a></p>"
						"</body></html>";
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
	const size_t chunkSize = 4096;
	size_t toWrite = std::min(chunkSize, conv.req.body.size());

	ssize_t written = write(conv.tempFd, conv.req.body.c_str(), toWrite);

	std::cout << "[resumePostWrite] written: " << written << std::endl;

	if (written < 0)
	{
		std::cerr << "[ERROR resumePostWrite] write failed with errno=" << errno << " (" << strerror(errno) << ")" << std::endl;
		conv.fdToClose = conv.tempFd;
		conv.tempFd = -1;
		std::cerr << "[ERROR resumePostWrite] written < 0: " << written << std::endl;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	conv.req.body.erase(0, written);
	std::cout << "[resumePostWriteBodyToCGI] Remaining body size after erase: " << conv.req.body.size() << std::endl;

	if (conv.req.body.empty())
	{
		conv.fdToClose = conv.tempFd;
		conv.tempFd = conv.bodyFd; //je switch et je stocke bodyFd dans tempFd (comme ca, il reste le seul a etre surveille par epoll)
		conv.bodyFd = -1;
		std::cout << "[resumePostWriteBodyToCGI] Finished writing body. Switching tempFd to bodyFd: " << conv.tempFd << std::endl;
		conv.eState = READ_EXEC_POST_CGI;
		conv.state = READ_EXEC;
	}
	else
	{
		conv.eState = WRITE_EXEC_POST_CGI;
		conv.state = WRITE_EXEC;
		std::cout << "[resumePostWriteBodyToCGI] More body to write. Staying in WRITE_EXEC_POST_CGI." << std::endl;
	}
}

void PostExecutor::resumePostReadCGI(Conversation& conv)
{
	char buffer[1024];
	ssize_t bytesRead = read(conv.tempFd, buffer, sizeof(buffer));

	std::cout << "[resumePostReadCGI] bytesRead: " << bytesRead << std::endl;

	if (bytesRead > 0)
	{
		conv.cgiOutput.append(buffer, bytesRead);
		std::cout << "[resumePostReadCGI] cgiOutput size: " << conv.cgiOutput.size() << std::endl;
	}
	else if (bytesRead == 0)
	{
		std::cout << "[resumePostReadCGI] EOF reached on fd " << conv.tempFd << std::endl;
		conv.fdToClose = conv.tempFd;
		conv.tempFd = -1;
		if (!hasCgiProcessExitedCleanly(conv.cgiPid))
		{
			std::cerr << "[resumePostReadCGI] CGI process did not exit cleanly." << std::endl;
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
		std::cerr << "[resumePostReadCGI] read error" << std::endl;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
	}
}
