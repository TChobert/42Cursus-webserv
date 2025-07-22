#include "CGIHandler.hpp"

bool CGIHandler::isCGI(const Conversation& conv)
{
	const std::string& path = conv.req.pathOnDisk;

	//Trouver l'extension
	std::size_t dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
		return false; //Aucun point = aucune extension = pas CGI

	//Extraire l'extension
	std::string extension = path.substr(dotPos);

	//Recup la map des CGI handlers
	const std::map<cgiExtension, cgiHandler>& cgiMap = conv.location->cgiHandlers;

	//Verif extension
	bool isCgiExtension = (cgiMap.find(extension) != cgiMap.end());

	if (isCgiExtension)
		return true;
	else
		return false;
}

void CGIHandler::handleCGI(const Conversation& conv)
{
	//check methode si GET ou POST...
}
