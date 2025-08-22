#include "GetExecutor.hpp"
#include "Executor.hpp"
#include "CGIHandler.hpp"
#include <unistd.h>
#include <dirent.h>
#include <sstream>

/* ---------------- PUBLIC METHODS ------------------ */

void	GetExecutor::handleGet(Conversation& conv)
{
	std::cout << "[handleGet] pathOnDisk: " << conv.req.pathOnDisk << std::endl;
	const std::string& path = conv.req.pathOnDisk;
	statusCode code = ResourceChecker::checkAccess(path);
	if (code != NOT_A_STATUS_CODE)
	{
		std::cout << "[handleGet] Access check failed with code: " << code << std::endl;
		Executor::setResponse(conv, code);
		return;
	}
	if (ResourceChecker::isFile(path)) {
		std::cout << "[handleGet] Detected file." << std::endl;
		handleFile(conv);
	}
	else if (ResourceChecker::isDir(path)) {
		std::cout << "[handleGet] Detected directory." << std::endl;
		handleDirectory(conv);
	}
	else {
		std::cout << "[handleGet] Neither file nor directory -> forbidden." << std::endl;
		Executor::sendErrorPage(conv, FORBIDDEN);
	}
}

void	GetExecutor::handleFile(Conversation& conv)
{
	std::cout << "[handleFile] pathOnDisk: " << conv.req.pathOnDisk << std::endl;
	if (CGIHandler::isCGI(conv))
		CGIHandler::handleCGI(conv);
	else
		StaticFileHandler::handleStaticFile(conv, OK);
}

void GetExecutor::handleDirectory(Conversation& conv)
{
	const std::string& dirPath = conv.req.pathOnDisk;
	const std::vector<std::string>& indexFiles = conv.location->indexFiles; // maintenant vecteur

	std::cout << "[handleDirectory] dirPath: " << dirPath << std::endl;
	for (size_t i = 0; i < indexFiles.size(); ++i)
	{
		std::string indexPath = dirPath + "/" + indexFiles[i];
		std::cout << "[handleDirectory] Checking index file: " << indexPath << std::endl;

		if (ResourceChecker::exists(indexPath) &&
			ResourceChecker::isFile(indexPath) &&
			ResourceChecker::isReadable(indexPath))
			{
				std::cout << "[handleDirectory] Found index file: " << indexPath << std::endl;
				conv.req.pathOnDisk = indexPath;
				handleFile(conv);
				return ;
			}
	}
	std::cout << "[handleDirectory] No index found. autoIndex: " << conv.location->autoIndex << std::endl;
	if (conv.location->autoIndex)
		handleAutoindex(conv);
	else
		Executor::sendErrorPage(conv, FORBIDDEN);;
}

// void	GetExecutor::handleDirectory(Conversation& conv)
// {
// 	const std::string& dirPath = conv.req.pathOnDisk;
// 	const std::string& indexName = conv.location->indexFiles;
// 	std::string indexPath = dirPath + "/" + indexName;

// 	if (!indexName.empty())
// 	{
// 		if (ResourceChecker::exists(indexPath) &&
// 			ResourceChecker::isFile(indexPath) &&
// 			ResourceChecker::isReadable(indexPath))
// 		{
// 			conv.req.pathOnDisk = indexPath;
// 			handleFile(conv);
// 			return;
// 		}
// 	}
// 	if (conv.location->autoIndex)
// 		handleAutoindex(conv);
// 	else
// 		Executor::setResponse(conv, FORBIDDEN);
// }

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

//EXEMPLE de lecture de resultat de script CGI sans parsing:
// Content-Type: text/html
// Set-Cookie: sessionid=123abc

// <html>
// <head><title>OK</title></head>
// <body>Hello from CGI</body>
// </html>

//donc il faut parser toute cette lecture et remettre dans les bonnes parties
//pour construire la reponse HTTP
// >> extraire headers et mettre dans les variables appropriees
// >> extraire body

// void GetExecutor::resumeReadCGI(Conversation& conv)
// {
// 	char buffer[1024];
// 	ssize_t bytesRead;
// 	while ((bytesRead = read(conv.cgiOut, buffer, sizeof(buffer))) > 0)
// 	{
// 		conv.cgiOutput.append(buffer, bytesRead);
// 	}
// 	if (bytesRead == 0)
// 	{
// 		conv.fdToClose = conv.cgiOut;
// 		conv.cgiOut= -1;
// 		if (!hasCgiProcessExitedCleanly(conv.cgiPid))
// 		{
// 			Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 			return;
// 		}
// 		CGIHandler::parseCgiOutput(conv);
// 		Executor::updateResponseData(conv);
// 		conv.eState = EXEC_START;
// 		conv.state = RESPONSE;
// 	}
// 	else if (bytesRead < 0)
// 	{
// 		conv.fdToClose = conv.cgiOut;

// 		conv.cgiOut= -1;
// 		Executor::setResponse(conv, INTERNAL_SERVER_ERROR);
// 	}
// }

