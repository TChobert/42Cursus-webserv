#ifndef BODYBUILDER_HPP
# define BODYBUILDER_HPP

# include <sstream>
# include "HttpResponse.hpp"
# include "StatusLineBuilder.hpp"

class BodyBuilder
{
	// private:
	// 	static std::string generateDefaultBody(int statusCode);

	public:
		static std::string build(HttpResponse& response);
};

#endif

