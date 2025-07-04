#include "HeaderBuilder.hpp"
#include "utils.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

std::string HeaderBuilder::buildGenericHeaders(const HttpResponse& response)
{
	std::ostringstream headers;

	headers << buildDateHeader();
	headers << buildContentLengthHeader(response);
	headers << buildContentTypeHeader(response);
	headers << buildConnectionHeader(response);

	headers << "\r\n";

	return headers.str();
}
std::string HeaderBuilder::buildDateHeader()
{
	return "Date: " + getCurrentHttpDate() + "\r\n";
}

std::string HeaderBuilder::buildContentLengthHeader(const HttpResponse& response)
{
	if (response.body.empty())
	{
		if (response.statusCode == 204 || response.statusCode == 304 || (response.statusCode >= 100 && response.statusCode < 200))
		{
			return "Content-Length: 0\r\n";
		}
		return "";
	}

	return "Content-Length: " + intToString(response.body.size()) + "\r\n";
}

std::string HeaderBuilder::buildContentTypeHeader(const HttpResponse& response)
{
	if (response.body.empty())
		return ""; // pas de Content-Length si pas de body

	return "Content-Type: " + response.contentType + "\r\n";
}

std::string HeaderBuilder::buildConnectionHeader(const HttpResponse& response)
{
	//check booleen shouldClose pour savoir si la conv doit close ou pas
	//!attention! reflechir sur update de ce booleen, une partie dans le parser si le client demande
	//une partie dans la partie metier (validator/exec) ? >> a voir...ou tout dans cette classe ?
	return response.shouldClose ? "Connection: close\r\n" : "Connection: keep-alive\r\n";
}

std::string HeaderBuilder::buildCustomHeaders(const HttpResponse& response)
{
	//a developper en fonction des redirections (301 etc)
	return "";
}

/* ---------------- PUBLIC METHODS ------------------ */

std::string HeaderBuilder::build(const HttpResponse& response)
{
	std::ostringstream headers;

	headers << HeaderBuilder::buildGenericHeaders(response);
	headers << HeaderBuilder::buildCustomHeaders(response);
	headers << "\r\n";

	return headers.str();
}
