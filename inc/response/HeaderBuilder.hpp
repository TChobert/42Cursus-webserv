#ifndef HEADERBUILDER_HPP
# define HEADERBUILDER_HPP

# include <string>
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"

class Conversation;
class HttpResponse;
class HttpRequest;

class HeaderBuilder
{
	private:
		static std::string buildGenericHeaders(const HttpResponse& response, const HttpRequest& request);
		static std::string buildCustomHeaders(const HttpResponse& response);

	public:
		static std::string build(const HttpResponse& response, const HttpRequest& request);
};

#endif
