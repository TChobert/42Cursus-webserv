#include "StatusLineBuilder.hpp"
#include "webserv_utils.hpp"

/* ---------------- PRIVATE ------------------ */

//map contenant tous les codes + leur message associe
std::map<int, std::string> StatusLineBuilder::createStatusTexts()
{
	std::map<int, std::string> m;

	//make_pair permet d'associer 2 elements sans specifier leur type (contrairement a pair)
	m.insert(std::make_pair(100, "Continue"));
	m.insert(std::make_pair(200, "OK"));
	m.insert(std::make_pair(201, "Created"));
	m.insert(std::make_pair(204, "No Content"));
	m.insert(std::make_pair(301, "Moved Permanently"));
	m.insert(std::make_pair(302, "Found"));
	m.insert(std::make_pair(400, "Bad Request"));
	m.insert(std::make_pair(403, "Forbidden"));
	m.insert(std::make_pair(404, "Not Found"));
	m.insert(std::make_pair(405, "Method Not Allowed"));
	m.insert(std::make_pair(408, "Request Timeout"));
	m.insert(std::make_pair(411, "Length Required"));
	m.insert(std::make_pair(413, "Payload Too Large"));
	m.insert(std::make_pair(431, "Request Header Fields Too Large"));
	m.insert(std::make_pair(500, "Internal Server Error"));
	m.insert(std::make_pair(501, "Not Implemented"));

	return m;
}

//init map
const std::map<int, std::string> StatusLineBuilder::_statusTexts = StatusLineBuilder::createStatusTexts();


/* ---------------- PUBLIC ------------------ */

//fonction de creation de la premiere ligne de reponse
// ex: "HTTP/1.1 200 OK\r\n"
std::string StatusLineBuilder::build(const HttpResponse& response)
{
	int	code = response.statusCode;

	std::string message = "Unknown"; //init pour legere protection
	std::map<int, std::string>::const_iterator it = _statusTexts.find(code);

	if (it != _statusTexts.end()) //trouver le  message associe au code
		message = it->second;

	std::string httpVersion = "HTTP/1.1";
	std::string statusCodeString = intToString(code);

	std::string statusLine = httpVersion + " " + statusCodeString + " " + message + "\r\n";

	return statusLine;
}
