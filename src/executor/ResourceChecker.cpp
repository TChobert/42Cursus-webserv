#include "ResourceChecker.hpp"

//stat renvoie 0 si le fichier existe, -1 si ca n'existe pas (ou pas accessible)

bool ResourceChecker::exists(const std::string& path)
{
	struct stat buffer;

	int result = stat(path.c_str(), &buffer);

	if (result == 0)
		return true;
	else
		return false;
}

bool ResourceChecker::isFile(const std::string& path)
{
	struct stat buffer;

	int result = stat(path.c_str(), &buffer);

	if (result != 0)
		return false;

	if (S_ISREG(buffer.st_mode)) //macro pour designer regular file
		return true;
	else
		return false;
}

bool ResourceChecker::isDir(const std::string& path)
{
	struct stat buffer;
	int result = stat(path.c_str(), &buffer);

	if (result != 0)
		return false;

	if (S_ISDIR(buffer.st_mode))
		return true;
	else
		return false;
}

bool ResourceChecker::isReadable(const std::string& path)
{
	int result = access(path.c_str(), R_OK);

	if (result == 0)
		return true;
	else
		return false;
}

bool ResourceChecker::isExecutable(const std::string& path)
{
	int result = access(path.c_str(), X_OK);

	if (result == 0)
		return true;
	else
		return false;
}

statusCode ResourceChecker::checkAccess(const std::string& path)
{
	if (!exists(path))
		return NOT_FOUND;
	if (!isFile(path) && !isDir(path))
		return FORBIDDEN;
	if (!isReadable(path))
		return FORBIDDEN;
	return NOT_A_STATUS_CODE;
}
