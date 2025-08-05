#pragma once

# include <string>
# include <sstream>
# include "webserv.hpp"

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

		static std::string buildCustomHeaders(const Conversation& conv);
		static std::string buildLocationHeader(const response& resp);
		static std::string buildSetCookieHeaders(const response& resp);
		static std::string buildAllowHeader(const Conversation& conv);

	public:
		static std::string build(const Conversation& conv);
};

//exemple d'utilisation du vector par executor/validator pour set-cookie
//response.setCookies.push_back("session_id=abc123; Path=/; HttpOnly");
