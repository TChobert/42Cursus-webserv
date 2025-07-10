#include "BodyBuilder.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

// std::string BodyBuilder::generateDefaultBody(int statusCode)
// {
// 	std::ostringstream	body;
// 	std::string			reason = StatusLineBuilder::getStatusText(statusCode);

// 	body << "<html><head><title>" << statusCode << " " << reason << "</title></head>";
// 	body << "<body><h1>" << statusCode << " " << reason << "</h1>";
// 	body << "<hr><p>webserv/1.0</p></body></html>";

// 	return body.str();
// }


/* ---------------- PUBLIC ------------------ */

std::string BodyBuilder::build(HttpResponse& response)
{
	// if (response.statusCode >= 400 && response.body.empty())
	// {
	// 	response.body = generateDefaultBody(response.statusCode);
	// }
	return response.body;
}




