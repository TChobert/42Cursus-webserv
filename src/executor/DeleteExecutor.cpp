#include "DeleteExecutor.hpp"
#include "Executor.hpp"
#include <algorithm>

void	DeleteExecutor::handleDelete(Conversation& conv)
{
	const std::string& path = conv.req.pathOnDisk;

	if (!ResourceChecker::exists(path))
		return Executor::sendErrorPage(conv, NOT_FOUND);
	if (!ResourceChecker::canDelete(conv.req.pathOnDisk)) //check droits d'ecriture sur le dossier parent
		return Executor::sendErrorPage(conv, FORBIDDEN);

	//checker la valeur de retour de unlink ou rmdir, si c'est 0 >> OK !
	int result;
	if (ResourceChecker::isFile(path))
		result = unlink(path.c_str());
	else if (ResourceChecker::isDir(path))
	{
		if (!ResourceChecker::isDirEmpty(path))
			return Executor::sendErrorPage(conv, FORBIDDEN);;
		result = rmdir(path.c_str());
	}
	else
		return Executor::sendErrorPage(conv, FORBIDDEN);;

	if (result == 0) //si on mettait un body html de confirmation, on mettrait 200 OK
	{
		conv.resp.body.clear();
		Executor::setResponse(conv, NO_CONTENT);
	}
	else
		Executor::sendErrorPage(conv, INTERNAL_SERVER_ERROR);
}

