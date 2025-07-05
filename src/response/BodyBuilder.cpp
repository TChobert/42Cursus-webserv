#include "BodyBuilder.hpp"

std::string BodyBuilder::build(const HttpResponse& response)
{
	return response.body;
}

//question du body lorsque chunk, a gerer ou pas ?
