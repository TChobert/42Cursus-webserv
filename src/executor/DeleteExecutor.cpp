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
	if (!ResourceChecker::canDelete(conv.req.pathOnDisk))
		return Executor::setResponse(conv, FORBIDDEN);


	// 4. Supprimer fichier ou dossier vide
	int ret;
	if (ResourceChecker::isFile(path))
		ret = unlink(path.c_str()); //autre chose a faire ? mis a part check valeur de retour ?
	else if (ResourceChecker::isDir(path)) //voir si c'est vide avant >> creer fonction pour dans ResourceChecker
		ret = rmdir(path.c_str());
	else
	{
		Executor::setResponse(conv, FORBIDDEN);
		return;
	}

	if (ret == 0)
	{
		conv.resp.body = "<html><body><h1>Deleted successfully</h1></body></html>";
		Executor::setResponse(conv, OK);
	}
	else
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
}

void	DeleteExecutor::resumeDelete(Conversation& conv)
{

}
