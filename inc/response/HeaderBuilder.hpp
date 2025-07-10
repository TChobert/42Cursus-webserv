#ifndef HEADERBUILDER_HPP
# define HEADERBUILDER_HPP

# include <string>
# include <sstream>
# include "Response.hpp"

class HttpResponse;

class HeaderBuilder
{
	private:
		static std::string buildGenericHeaders(const Response& resp);
		static std::string buildDateHeader();
		static std::string buildContentLengthHeader(const Response& resp);
		static std::string buildContentTypeHeader(const Response& resp);
		static std::string buildServerHeader();
		static std::string buildConnectionHeader(const Response& resp);
		static bool isBodyForbidden(int statusCode);

		static std::string buildCustomHeaders(const Response& resp);
		static std::string buildLocationHeader(const Response& resp);
		static std::string buildSetCookieHeaders(const Response& resp);
		static std::string buildAllowHeader(const Response& resp);

	public:
		static std::string build(const Response& resp);
};

#endif

//exemple d'utilisation du vector par executor/validator pour set-cookie
//response.setCookies.push_back("session_id=abc123; Path=/; HttpOnly");

//exemple d'utilisation du vector par executor/validator pour allowedMethods
//response.statusCode = 405;
//response.allowedMethods.push_back("GET");
//response.allowedMethods.push_back("POST");

//HTTP/1.1 405 Method Not Allowed
//Allow: GET, POST
