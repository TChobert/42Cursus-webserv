#include "GetExecutor.hpp"

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
		conv.resp.status = FORBIDDEN;
}

void	GetExecutor::handleAutoindex(Conversation& conv)
{
	//ATTENTION operation write body : fd a envoyer a epoll...
}

void GetExecutor::resumeStatic(Conversation&)
{

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
