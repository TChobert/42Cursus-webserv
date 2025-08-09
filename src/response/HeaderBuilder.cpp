#include "HeaderBuilder.hpp"
#include "webserv_utils.hpp"
#include "webserv_enum.hpp"

/* ---------------- PRIVATE METHODS ------------------ */


std::string HeaderBuilder::buildGenericHeaders(const Conversation& conv)
{
	std::ostringstream headers;

	headers << buildDateHeader();
	headers << buildServerHeader();
	headers << buildContentTypeHeader(conv.resp);
	if (conv.streamState == STREAM_IN_PROGRESS || conv.streamState == START_STREAM)
	{
		headers << "Transfer-Encoding: chunked\r\n";
	}
	else
	{
		headers << buildContentLengthHeader(conv.resp);
	}
	headers << buildConnectionHeader(conv.resp);

	return headers.str();
}

std::string HeaderBuilder::buildCustomHeaders(const Conversation& conv)
{
	std::ostringstream headers;

	headers << buildLocationHeader(conv.resp);
	headers << buildSetCookieHeaders(conv.resp); //attention il doit y avoir 1 ligne pour chaque cookie
	headers << buildAllowHeader(conv);

	return headers.str();
}

std::string HeaderBuilder::buildDateHeader()
{
	return "Date: " + getCurrentHttpDate() + "\r\n";
}

std::string HeaderBuilder::buildContentLengthHeader(const response& resp)
{
	if (isBodyForbidden(resp.status))
		return "";
	// Si body vide, mais code qui accepte body
	if (resp.body.empty())
	{
		return "Content-Length: 0\r\n";
	}
	return "Content-Length: " + intToString(resp.body.size()) + "\r\n";
}

std::string HeaderBuilder::buildContentTypeHeader(const response& resp)
{
	if (isBodyForbidden(resp.status) || resp.body.empty())
		return "";
	return "Content-Type: " + resp.contentType + "\r\n";
}

std::string HeaderBuilder::buildConnectionHeader(const response& resp)
{
	return resp.shouldClose ? "Connection: close\r\n" : "Connection: keep-alive\r\n";
}

std::string HeaderBuilder::buildServerHeader()
{
	return "Server: webserv/1.0\r\n";
}

std::string HeaderBuilder::buildLocationHeader(const response& resp)
{
	if (resp.location.empty())
		return "";

	int code = resp.status;

	if (code == CREATED || (code >= 300 && code < 400))
		return "Location: " + resp.location + "\r\n";

	return "";
}

std::string HeaderBuilder::buildSetCookieHeaders(const response& resp)
{
	std::ostringstream headers;

	for (std::vector<std::string>::const_iterator it = resp.setCookies.begin(); it != resp.setCookies.end(); ++it)
	{
		headers << "Set-Cookie: " << *it << "\r\n";
	}

	return headers.str();
}

std::string HeaderBuilder::buildAllowHeader(const Conversation& conv)
{
	if ((conv.resp.status != METHOD_NOT_ALLOWED && conv.resp.status != NOT_IMPLEMENTED)
		|| !conv.location || conv.location->allowedMethods.empty())
		return "";

	std::ostringstream line;
	line << "Allow: ";

	const std::vector<std::string>& methods = conv.location->allowedMethods;

	for (size_t i = 0; i < methods.size(); ++i)
	{
		line << methods[i];
		if (i != methods.size() - 1)
			line << ", ";
	}

	line << "\r\n";
	return line.str();
}

/* ---------------- PUBLIC METHODS ------------------ */

std::string HeaderBuilder::build(const Conversation& conv)
{
	std::ostringstream headers;

	headers << HeaderBuilder::buildGenericHeaders(conv);
	headers << HeaderBuilder::buildCustomHeaders(conv);

	return headers.str();
}

bool HeaderBuilder::isBodyForbidden(int statusCode)
{
	return ((statusCode >= 100 && statusCode < 200)
		|| statusCode == NO_CONTENT || statusCode == 304);
}
