#include "Executor.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

void	Executor::handleGet(Conversation& conv)
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

void	Executor::handlePost(Conversation& conv)
{

}

void	Executor::handleDelete(Conversation& conv)
{

}

void	Executor::handleFile(Conversation& conv)
{
	if (CGIHandler::isCGI(conv))
		CGIHandler::handleCGI(conv);
	else
		StaticFileHandler::handleStaticFile(conv);
}

void	Executor::handleDirectory(Conversation& conv)
{

}

/* ---------------- PUBLIC METHODS ------------------ */

void	Executor::execute(Conversation& conv)
{
	if (conv.resp.status != NOT_A_STATUS_CODE && conv.resp.status != CONTINUE)
		return;

	const std::string& method = conv.req.method;

	if (method == "GET")
		handleGet(conv);
	else if (method == "POST")
		handlePost(conv);
	else if (method == "DELETE")
		handleDelete(conv);
	else
		conv.resp.status = NOT_IMPLEMENTED;
}
