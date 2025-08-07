#include "StaticFileHandler.hpp"
#include "Executor.hpp"
#include "webserv.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>

void StaticFileHandler::handleStaticFile(Conversation& conv)
{
    const std::string& path = conv.req.pathOnDisk;

    // 1. Vérifier que le fichier existe et obtenir sa taille
    struct stat st;
    if (stat(path.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
        Executor::setResponse(conv, NOT_FOUND);
        return;
    }

    // 2. Vérifier que la taille est acceptable
    if (st.st_size > MAX_SAFE_SIZE) {
        Executor::setResponse(conv, ENTITY_TOO_LARGE);
        return;
    }

    // 3. Ouvrir avec ifstream et tout lire
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf(); // lire tout le contenu dans le stringstream
    conv.resp.body = ss.str();

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
