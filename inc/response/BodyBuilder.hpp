#ifndef BODYBUILDER_HPP
# define BODYBUILDER_HPP

# include "HttpResponse.hpp"

class BodyBuilder
{
	private:

	public:
		static std::string BodyBuilder::build(const HttpResponse& response);
};

#endif

