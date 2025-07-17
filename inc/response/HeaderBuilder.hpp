#pragma once

# include <string>
# include <sstream>
# include "webserv.hpp"

class HttpResponse;

class HeaderBuilder
{
	private:
		static std::string buildGenericHeaders(const response& resp);
		static std::string buildDateHeader();
		static std::string buildContentLengthHeader(const response& resp);
		static std::string buildContentTypeHeader(const response& resp);
		static std::string buildServerHeader();
		static std::string buildConnectionHeader(const response& resp);
		static bool isBodyForbidden(int statusCode);

		static std::string buildCustomHeaders(const response& resp);
		static std::string buildLocationHeader(const response& resp);
		static std::string buildSetCookieHeaders(const response& resp);
		static std::string buildAllowHeader(const response& resp);

	public:
		static std::string build(const response& resp);
};

//exemple d'utilisation du vector par executor/validator pour set-cookie
//response.setCookies.push_back("session_id=abc123; Path=/; HttpOnly");

//exemple d'utilisation du vector par executor/validator pour allowedMethods
//response.statusCode = 405;
//response.allowedMethods.push_back("GET");
//response.allowedMethods.push_back("POST");

//HTTP/1.1 405 Method Not Allowed
//Allow: GET, POST
