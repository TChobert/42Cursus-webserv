#include "PostExecutor.hpp"
#include "Executor.hpp"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

struct MultipartPart
{
	std::string name;
	std::string filename;
	std::string contentType;
	std::string data;
};

static std::string	generateUploadFileName()
{
	static int counter = 0;
	std::ostringstream ss;
	ss << "upload_" << counter++ << ".txt";
	return ss.str();
}

static std::string sanitizeFilename(const std::string& raw)
{
	std::string clean;
	for (std::string::size_type i = 0; i < raw.size(); ++i)
	{
		if (std::isalnum(static_cast<unsigned char>(raw[i])) || raw[i] == '.' || raw[i] == '_' || raw[i] == '-')
		{
			clean += raw[i];
		}
	}
	return clean.empty() ? generateUploadFileName() : clean;
}

std::vector<std::string> splitMultipartBody(const std::string& body, const std::string& boundary)
{
	std::vector<std::string> parts;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = body.find(boundary, start)) != std::string::npos)
	{
		if (end != start)
		{
			std::string part = body.substr(start, end - start);
			if (!part.empty())
				parts.push_back(part);
		}
		start = end + boundary.length();
	}
	return parts;
}

MultipartPart parsePart(const std::string& part)
{
	MultipartPart parsed;
	std::string::size_type headerEnd = part.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return parsed;

	std::string headerSection = part.substr(0, headerEnd);
	std::string body = part.substr(headerEnd + 4); // skip \r\n\r\n

	std::istringstream headers(headerSection);
	std::string line;
	while (std::getline(headers, line))
	{
		if (line.find("Content-Disposition:") != std::string::npos)
		{
			std::string::size_type namePos = line.find("name=\"");
			if (namePos != std::string::npos)
			{
				std::string::size_type end = line.find("\"", namePos + 6);
				parsed.name = line.substr(namePos + 6, end - (namePos + 6));
			}
			std::string::size_type filePos = line.find("filename=\"");
			if (filePos != std::string::npos)
			{
				std::string::size_type end = line.find("\"", filePos + 10);
				parsed.filename = line.substr(filePos + 10, end - (filePos + 10));
			}
		}
		else if (line.find("Content-Type:") != std::string::npos)
		{
			parsed.contentType = line.substr(14); // after "Content-Type: "
			if (!parsed.contentType.empty() &&
				parsed.contentType[parsed.contentType.size() - 1] == '\r')
			{
				parsed.contentType.erase(parsed.contentType.size() - 1);
			}
		}
	}

	// Trim trailing CRLF or dashes
	while (!body.empty() && (body[body.size() - 1] == '\r' ||
							 body[body.size() - 1] == '\n' ||
							 body[body.size() - 1] == '-'))
	{
		body.erase(body.size() - 1);
	}

	parsed.data = body;
	return parsed;
}

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
	std::vector<std::string> parts = splitMultipartBody(conv.req.body, boundary);

	std::vector<std::string>::size_type i;
	for (i = 0; i < parts.size(); ++i)
	{
		MultipartPart part = parsePart(parts[i]);

		if (!part.filename.empty())
		{
			std::string filename = sanitizeFilename(part.filename);
			std::string path = conv.location->uploadStore + "/" + filename;

			std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
			if (!file.is_open())
			{
				Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
				return;
			}
			file.write(part.data.c_str(), static_cast<std::streamsize>(part.data.size()));
			file.close();
		}
	}

	conv.resp.body = "<html><body><h1>Upload successful</h1></body></html>";
	Executor::setResponse(conv, CREATED);
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
