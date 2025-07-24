#include "DeleteExecutor.hpp"
#include "Executor.hpp"
#include <algorithm>

bool	isMethodAllowed(const locationConfig* location, const std::string& method)
{
	if (!location) //s'il n'y a pas de location, Delete pas autorise.. peut etre adapte si besoin
		return false;
	return std::find(location->allowedMethods.begin(), location->allowedMethods.end(), method) != location->allowedMethods.end();
}

void	DeleteExecutor::handleDelete(Conversation& conv)
{
	const std::string& path = conv.req.pathOnDisk;

	if (!ResourceChecker::exists(path))
		return Executor::setResponse(conv, NOT_FOUND);
	if (!isMethodAllowed(conv.location, "DELETE"))
		return Executor::setResponse(conv, METHOD_NOT_ALLOWED);
	if (!ResourceChecker::canDelete(conv.req.pathOnDisk)) //check droits d'ecriture sur le dossier parent
		return Executor::setResponse(conv, FORBIDDEN);

	//checker la valeur de retour de unlink ou rmdir, si c'est 0 >> OK !
	int result;
	if (ResourceChecker::isFile(path))
		result = unlink(path.c_str());
	else if (ResourceChecker::isDir(path))
	{
		if (!ResourceChecker::isDirEmpty(path))
			return Executor::setResponse(conv, FORBIDDEN);
		result = rmdir(path.c_str());
	}
	else
		return Executor::setResponse(conv, FORBIDDEN);

	if (result == 0) //si on mettait un body html de confirmation, on mettrait 200 OK
	{
		conv.resp.body.clear();
		Executor::setResponse(conv, NO_CONTENT);
	}
	else
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
}

