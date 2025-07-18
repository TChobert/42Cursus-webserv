#include "GetExecutor.hpp"
#include <unistd.h>

void	GetExecutor::handleGet(Conversation& conv)
{
	const std::string& path = conv.req.uri;

	statusCode code = ResourceChecker::checkAccess(path);
	if (code != NOT_A_STATUS_CODE)
	{
		conv.resp.status = code;
		return;
	}
	if (ResourceChecker::isFile(path))
		handleFile(conv);
	else if (ResourceChecker::isDir(path))
		handleDirectory(conv);
	else
	//ATTENTION: MAJ Struct Response?
		conv.resp.status = FORBIDDEN;
}

void	GetExecutor::handleFile(Conversation& conv)
{
	if (CGIHandler::isCGI(conv))
		CGIHandler::handleCGI(conv);
	else
		StaticFileHandler::handleStaticFile(conv);
}

void	GetExecutor::handleDirectory(Conversation& conv)
{
	const std::string& dirPath = conv.req.uri;
	const std::string& indexName = conv.location->index;
	std::string indexPath = dirPath + "/" + indexName;

	if (!indexName.empty())
	{
		if (ResourceChecker::exists(indexPath) &&
			ResourceChecker::isFile(indexPath) &&
			ResourceChecker::isReadable(indexPath))
		{
			conv.req.uri = indexPath;
			handleFile(conv);
			return;
		}
	}
	if (conv.location->autoIndex)
		handleAutoindex(conv);
	else
	//ATTENTION: MAJ Struct Response?
		conv.resp.status = FORBIDDEN;
}

void	GetExecutor::handleAutoindex(Conversation& conv)
{
	//ATTENTION operation write body : fd a envoyer a epoll...
}

void GetExecutor::resumeStatic(Conversation& conv)
{
	const size_t BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	ssize_t bytesRead = read(conv.tempFd, buffer, BUFFER_SIZE);

	if (bytesRead > 0)
	{
		conv.resp.body.append(buffer, bytesRead);
		return; //on attend prochain EPOLLIN (encore donnees a lire)
	}
	else if (bytesRead == 0)
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		conv.resp.status = OK;
		//ATTENTION: MAJ Struct Response?
		conv.state = RESPONSE;
		return;
	}
	else
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		conv.resp.status = INTERNAL_SERVER_ERROR;
		//ATTENTION: MAJ Struct Response?
		conv.state = RESPONSE;
		return;
	}
	//autre chose a faire ?? Content-Type ? Content-Length ?
}

void GetExecutor::resumeAutoIndex(Conversation&)
{

}

void GetExecutor::resumeReadCGI(Conversation&)
{

}

void GetExecutor::resumeWriteCGI(Conversation&)
{

}
