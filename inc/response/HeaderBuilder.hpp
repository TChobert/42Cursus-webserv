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

		static std::string buildCustomHeaders(const HttpResponse& response);

	public:
		static std::string build(const HttpResponse& response);
};

#endif
