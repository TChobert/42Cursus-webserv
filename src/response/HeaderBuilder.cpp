#include "HeaderBuilder.hpp"
#include "webserv_utils.hpp"
#include "webserv_enum.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

bool HeaderBuilder::isBodyForbidden(int statusCode)
{
	return ((statusCode >= 100 && statusCode < 200)
		|| statusCode == NO_CONTENT || statusCode == 304);
}

std::string HeaderBuilder::buildGenericHeaders(const Response& resp)
{
	std::ostringstream headers;

	headers << buildDateHeader();
	headers << buildServerHeader();
	headers << buildContentTypeHeader(resp);
	headers << buildContentLengthHeader(resp);
	headers << buildConnectionHeader(resp);

	return headers.str();
}

std::string HeaderBuilder::buildCustomHeaders(const Response& resp)
{
	std::ostringstream headers;

	headers << buildLocationHeader(resp);
	headers << buildSetCookieHeaders(resp); //attention il doit y avoir 1 ligne pour chaque cookie
	headers << buildAllowHeader(resp);

	return headers.str();
}

std::string HeaderBuilder::buildDateHeader()
{
	return "Date: " + getCurrentHttpDate() + "\r\n";
}

std::string HeaderBuilder::buildContentLengthHeader(const Response& resp)
{
	if (isBodyForbidden(resp.statusCode))
		return "";
	// Si body vide, mais code qui accepte body
	if (resp.body.empty())
	{
		return "Content-Length: 0\r\n";
	}
	return "Content-Length: " + intToString(resp.body.size()) + "\r\n";
}

std::string HeaderBuilder::buildContentTypeHeader(const Response& resp)
{
	if (isBodyForbidden(resp.statusCode) || resp.body.empty())
		return "";
	return "Content-Type: " + resp.contentType + "\r\n";
}

std::string HeaderBuilder::buildConnectionHeader(const Response& resp)
{
	return resp.shouldClose ? "Connection: close\r\n" : "Connection: keep-alive\r\n";
}

std::string HeaderBuilder::buildServerHeader()
{
	return "Server: webserv/1.0\r\n";
}

std::string HeaderBuilder::buildLocationHeader(const Response& resp)
{
	if (resp.location.empty())
		return "";

	int code = resp.statusCode;

	if (code == CREATED || (code >= 300 && code < 400))
		return "Location: " + resp.location + "\r\n";

	return "";
}

std::string HeaderBuilder::buildSetCookieHeaders(const Response& resp)
{
	std::ostringstream headers;

	for (std::vector<std::string>::const_iterator it = resp.setCookies.begin(); it != resp.setCookies.end(); ++it)
	{
		headers << "Set-Cookie: " << *it << "\r\n";
	}

	return headers.str();
}

std::string HeaderBuilder::buildAllowHeader(const Response& resp) //changer avec conversation > stocke dans _config.locationConfig.allowedMethods
{
	if ((resp.statusCode != METHOD_NOT_ALLOWED && resp.statusCode != NOT_IMPLEMENTED)
		|| resp.allowedMethods.empty())
		return "";

	std::ostringstream line;
	line << "Allow: ";

	for (size_t i = 0; i < resp.allowedMethods.size(); ++i)
	{
		line << resp.allowedMethods[i];
		if (i != resp.allowedMethods.size() - 1)
			line << ", ";
	}

	line << "\r\n";
	return line.str();
}

/* ---------------- PUBLIC METHODS ------------------ */

std::string HeaderBuilder::build(const Response& resp)
{
	std::ostringstream headers;

	headers << HeaderBuilder::buildGenericHeaders(resp);
	headers << HeaderBuilder::buildCustomHeaders(resp);

	return headers.str();
}
