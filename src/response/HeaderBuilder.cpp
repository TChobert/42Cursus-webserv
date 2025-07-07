#include "HeaderBuilder.hpp"
#include "utils.hpp"

enum statusCode
{
	NOT_A_STATUS_CODE = 0,
	CONTINUE = 100,
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	TIMEOUT = 408,
	LENGTH_REQUIRED = 411,
	ENTITY_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501
};

/* ---------------- PRIVATE METHODS ------------------ */

std::string HeaderBuilder::buildGenericHeaders(const HttpResponse& response)
{
	std::ostringstream headers;

	headers << buildDateHeader();
	headers << buildServerHeader();
	headers << buildContentTypeHeader(response);
	headers << buildContentLengthHeader(response);
	headers << buildConnectionHeader(response);

	return headers.str();
}

std::string HeaderBuilder::buildCustomHeaders(const HttpResponse& response)
{
	std::ostringstream headers;

	headers << buildLocationHeader(response);
	headers << buildSetCookieHeaders(response); //attention il doit y avoir 1 ligne pour chaque cookie
	headers << buildAllowHeader(response);

	return headers.str();
}

std::string HeaderBuilder::buildDateHeader()
{
	return "Date: " + getCurrentHttpDate() + "\r\n";
}

std::string HeaderBuilder::buildContentLengthHeader(const HttpResponse& response)
{
	// Les codes qui n'ont jamais de body
	if (response.statusCode == NO_CONTENT || (response.statusCode >= 100 && response.statusCode < 200))
	{
		return "Content-Length: 0\r\n";
	}
	// Si body vide, mais code qui accepte body
	if (response.body.empty())
	{
		return "Content-Length: 0\r\n";
	}
	return "Content-Length: " + intToString(response.body.size()) + "\r\n";
}

std::string HeaderBuilder::buildContentTypeHeader(const HttpResponse& response)
{
	// Pas de Content-Type si pas de body ou si code qui interdit body
	if (response.body.empty() || response.statusCode == NO_CONTENT || (response.statusCode >= 100 && response.statusCode < 200))
	{
		return "";
	}
	return "Content-Type: " + response.contentType + "\r\n";
}

std::string HeaderBuilder::buildConnectionHeader(const HttpResponse& response)
{
	//check booleen shouldClose pour savoir si la conv doit close ou pas
	//!attention! reflechir sur update de ce booleen, une partie dans le parser si le client demande
	//une partie dans la partie metier (validator/exec) ? >> a voir...ou tout dans cette classe ?
	return response.shouldClose ? "Connection: close\r\n" : "Connection: keep-alive\r\n";
}

std::string HeaderBuilder::buildServerHeader()
{
	return "Server: webserv/1.0\r\n";
}

std::string HeaderBuilder::buildLocationHeader(const HttpResponse& response)
{
	if (response.location.empty())
		return "";

	return "Location: " + response.location + "\r\n";
}

std::string HeaderBuilder::buildSetCookieHeaders(const HttpResponse& response)
{
	std::ostringstream headers;

	for (std::vector<std::string>::const_iterator it = response.setCookies.begin(); it != response.setCookies.end(); ++it)
	{
		headers << "Set-Cookie: " << *it << "\r\n";
	}

	return headers.str();
}

std::string HeaderBuilder::buildAllowHeader(const HttpResponse& response)
{
	if (response.allowedMethods.empty())
		return "";

	std::ostringstream line;
	line << "Allow: ";

	for (size_t i = 0; i < response.allowedMethods.size(); ++i)
	{
		line << response.allowedMethods[i];
		if (i != response.allowedMethods.size() - 1)
			line << ", ";
	}

	line << "\r\n";
	return line.str();
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
