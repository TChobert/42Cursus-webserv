#include "StaticFileHandler.hpp"
#include "Executor.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>

void StaticFileHandler::handleStaticFile(Conversation& conv)
{
    const std::string& path = conv.req.pathOnDisk;
    int fd = open(path.c_str(), O_RDONLY);  // Pas de O_NONBLOCK
    if (fd == -1) {
        Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
        return;
    }

    // Lire tout le fichier maintenant
    std::string content;
    char buffer[8192];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }
    close(fd);

    if (bytesRead == -1) {
        Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
        return;
    }

    // Préparer la réponse
    conv.resp.body = content;
	Executor::setResponse(conv, OK);
}

// void	StaticFileHandler::handleStaticFile(Conversation& conv)
// {
// 	const std::string& path = conv.req.pathOnDisk;

// 	int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
// 	if (fd == -1)
// 	{
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 		return;
// 	}

// 	conv.tempFd = fd;
// 	conv.eState = READ_EXEC_GET_STATIC;
// 	conv.state = READ_EXEC;
// 	return;
// }

// ------ Version TEST below ------

// void StaticFileHandler::handleStaticFile(Conversation& conv)
// {
// 	const std::string& path = conv.req.pathOnDisk;

// 	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
// 	if (!file) {
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 		return;
// 	}

// 	std::ostringstream content;
// 	content << file.rdbuf();
// 	conv.resp.body = content.str();

// 	conv.resp.status = OK;
// 	conv.resp.contentType = getMimeType(path); // si dispo, sinon "text/plain"
// 	conv.state = RESPONSE;
// }
