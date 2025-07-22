#include "GetExecutor.hpp"
#include "Executor.hpp"
#include <unistd.h>
#include <dirent.h>
#include <sstream>

void	GetExecutor::handleGet(Conversation& conv)
{
	if (conv.location->hasRedir)
	{
		conv.resp.status = conv.location->redirCode;
		conv.resp.location = conv.location->redirURL;
		Executor::updateResponseData(conv);
		return;
	}

	const std::string& path = conv.req.pathOnDisk;
	statusCode code = ResourceChecker::checkAccess(path);
	if (code != NOT_A_STATUS_CODE)
	{
		Executor::setResponse(conv, code);
		return;
	}
	if (ResourceChecker::isFile(path))
		handleFile(conv);
	else if (ResourceChecker::isDir(path))
		handleDirectory(conv);
	else
		Executor::setResponse(conv, FORBIDDEN);
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
	const std::string& dirPath = conv.req.pathOnDisk;
	const std::string& indexName = conv.location->index;
	std::string indexPath = dirPath + "/" + indexName;

	if (!indexName.empty())
	{
		if (ResourceChecker::exists(indexPath) &&
			ResourceChecker::isFile(indexPath) &&
			ResourceChecker::isReadable(indexPath))
		{
			conv.req.pathOnDisk = indexPath;
			handleFile(conv);
			return;
		}
	}
	if (conv.location->autoIndex)
		handleAutoindex(conv);
	else
		Executor::setResponse(conv, FORBIDDEN);
}

//IMPORTANCE DISTINCTION URI / PATHONDISK pour AutoIndex

// DIR* dir = opendir("/var/www/public/documents/"); == pathOnDisk

//mais pour generer les bons liens HTML, je dois ecrire:
// <a href="/files/documents/file1.txt">

// ET PAS : /var/www/public/documents/file1.txt
// navigateur ne fonctionne qu'avec URI, comprend pas chemin systeme

std::string generateAutoindexPage(const Conversation& conv, const std::string& dirPath, const std::vector<std::string>& entries)
{
	std::ostringstream body;
	body << "<html><head><title>Index of " << dirPath << "</title></head><body>";
	body << "<h1>Index of " << dirPath << "</h1><ul>";

	std::string baseUri = conv.req.uri;
	if (!baseUri.empty() && baseUri[baseUri.size() - 1] != '/')
		baseUri += "/";

	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::string fullPath = dirPath + "/" + entries[i];
		bool isDir = ResourceChecker::isDir(fullPath);

		std::string href = baseUri + entries[i];
		if (isDir)
			href += "/";

		body << "<li><a href=\"" << href << "\">" << entries[i];
		if (isDir)
			body << "/";
		body << "</a></li>";
	}

	body << "</ul></body></html>";
	return body.str();
}

//on doit pouvoir lister tous les fichiers et dossiers
void	GetExecutor::handleAutoindex(Conversation& conv)
{
	//ouverture du dossier qui est dans le pathOnDisk
	const std::string& dirPath = conv.req.pathOnDisk;
	DIR* dir = opendir(dirPath.c_str()); //pointeur opaque vers un DIR = flux de repertoire ouvert par opendir()
	if (!dir)
	{
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}

	//lister et stocker les fichiers/dossiers dans un tableau entries[]
	std::vector<std::string> entries;
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		entries.push_back(name);
	}
	closedir(dir);

	//generer la page HTML Autoindex de facon dynamique avec la liste des fichiers/dossiers
	conv.resp.body = generateAutoindexPage(conv, dirPath, entries);
	conv.resp.contentType = "text/html";
	conv.resp.status = OK;
	conv.state = RESPONSE;
	return;
}

// ----- POUR INFO -----
// struct dirent
//{
//		ino_t	d_ino;       // numéro d'inode du fichier
//		char	d_name[256]; // nom du fichier (attention : longueur max)
//	+ autres champs
//};

void GetExecutor::resumeStatic(Conversation& conv)
{
	const size_t BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	ssize_t bytesRead = read(conv.tempFd, buffer, BUFFER_SIZE);

	if (bytesRead > 0)
	{
		conv.resp.body.append(buffer, bytesRead);
		conv.state = READ_EXEC;
		conv.eState = READ_EXEC_GET_STATIC;
		return; //on attend prochain EPOLLIN (encore donnees a lire)
	}
	else if (bytesRead == 0)
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		Executor::setResponse(conv, OK);
		return;
	}
	else
	{
		close(conv.tempFd);
		conv.tempFd = -1;
		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
		return;
	}
}

void GetExecutor::resumeReadCGI(Conversation&)
{

}

void GetExecutor::resumeWriteCGI(Conversation&)
{

}
