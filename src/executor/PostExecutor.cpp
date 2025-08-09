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
		conv.streamState = NORMAL;
		std::cout << "[resumePostWriteBodyToCGI] Finished writing body. Switching tempFd to bodyFd: " << conv.tempFd << std::endl;
		conv.eState = READ_EXEC_POST_CGI;
		conv.state = READ_EXEC;
	}
	else
	{
		conv.streamState = START_STREAM;
		conv.eState = WRITE_EXEC_POST_CGI;
		conv.state = WRITE_EXEC;
		std::cout << "[resumePostWriteBodyToCGI] More body to write. Staying in WRITE_EXEC_POST_CGI." << std::endl;
	}
}

void PostExecutor::resumePostReadCGI(Conversation& conv)
{
	char buffer[4096];
	ssize_t bytesRead = read(conv.tempFd, buffer, sizeof(buffer));

	std::cout << "[resumePostReadCGI] bytesRead: " << bytesRead << std::endl;

	if (bytesRead > 0)
    {
        // On ajoute les données au buffer brut CGI
        conv.cgiOutput.append(buffer, bytesRead);
        std::cout << "[resumePostReadCGI] cgiOutput size: " << conv.cgiOutput.size() << std::endl;

        if (conv.streamState == START_STREAM)
        {
            // Première lecture dans un contexte streaming → parse + envoyer headers + début du body
            CGIHandler::parseCgiOutput(conv);

            conv.streamState = STREAM_IN_PROGRESS;
        }
        else if (conv.streamState == STREAM_IN_PROGRESS)
        {
            // En streaming, on append directement au body (pas de re-parse)
            conv.resp.body.append(buffer, bytesRead);
        }
		Executor::updateResponseData(conv);
        conv.state = RESPONSE;
    }
    else if (bytesRead == 0)
    {
        std::cout << "[resumePostReadCGI] EOF reached on fd " << conv.tempFd << std::endl;
        conv.fdToClose = conv.tempFd;
        conv.tempFd = -1;
		conv.cgiFinished = true; 
        if (!hasCgiProcessExitedCleanly(conv.cgiPid))
        {
            std::cerr << "[resumePostReadCGI] CGI process did not exit cleanly." << std::endl;
            Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
            return;
        }

        if (conv.streamState == NORMAL)
        {
            // Pas de streaming → parse seulement à la fin
            CGIHandler::parseCgiOutput(conv);
        }
        // STREAM_IN_PROGRESS → on a déjà envoyé le body au fur et à mesure

        Executor::updateResponseData(conv);
        conv.streamState = NORMAL; // Reset pour la prochaine requête
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
}
