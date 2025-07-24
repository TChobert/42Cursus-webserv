#include "ResourceChecker.hpp"
#include <dirent.h>

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
	if (!isReadable(path))
		return FORBIDDEN;
	return NOT_A_STATUS_CODE;
}

//pour supprimer un fichier ex: parentDir/fichier.txt
//systeme a besoin de modifier le contenu du dossier parent
//car c'est dans ce dossier qu'on enleve l'entree "fichier.txt"
//>>> ce qui compte: droite d'ecriture sur le dossier parent

bool ResourceChecker::canDelete(const std::string& path)
{
	//derniere occurrence de '/' pour trouver le dossier parent
	std::size_t pos = path.find_last_of('/');

	if (pos == std::string::npos)
		return false;

	std::string parentDir = path.substr(0, pos);

	if (parentDir.empty())
		parentDir = "/";

	// check si on a le droit d’ecrire dans le dossier parent
	if (access(parentDir.c_str(), W_OK) != 0)
		return false;

	return true;
}

bool ResourceChecker::isDirEmpty(const std::string& path)
{
	DIR* dir = opendir(path.c_str());
	if (!dir)
		return false;

	//lister et stocker les fichiers/dossiers dans un tableau entries[]
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) //readdir lit chaque entree dans le dossier ouvert
	{
		std::string name(entry->d_name);
		if (name != "." && name != "..") //la on a trouve autre chose que le dossier lui-meme ou dossier parent
		{
			closedir(dir);
			return false;
		}
	}
	closedir(dir);
	return true;
}
