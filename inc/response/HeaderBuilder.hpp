#ifndef HEADERBUILDER_HPP
# define HEADERBUILDER_HPP

# include <string>
# include <sstream>
# include "HttpResponse.hpp"

class HttpResponse;

class HeaderBuilder
{
	private:
		static std::string buildGenericHeaders(const HttpResponse& response);
		static std::string buildDateHeader();
		static std::string buildContentLengthHeader(const HttpResponse& response);
		static std::string buildContentTypeHeader(const HttpResponse& response);
		static std::string buildServerHeader();
		static std::string buildConnectionHeader(const HttpResponse& response);
		//question du body lorsque chunk, a gerer ou pas ?
		// dans ce cas, prevoir Transfer-Encoding: chunked (et mieux gerer body ?)

		static std::string buildCustomHeaders(const HttpResponse& response);
		static std::string buildLocationHeader(const HttpResponse& response);
		static std::string buildSetCookieHeaders(const HttpResponse& response);
		static std::string buildAllowHeader(const HttpResponse& response);

	public:
		static std::string build(const HttpResponse& response);
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
