#include "ResponseBuilder.hpp"

std::string	ResponseBuilder::build(Conversation& conv)
{
	HttpResponse& response = conv.response;

	std::string statusLine = StatusLineBuilder::build(response);
	std::string headers = HeaderBuilder::build(response);
	std::string body = BodyBuilder::build(response);

	return (ResponseAssembler::assemble(statusLine, headers, body));
}

//a voir si on return direct ou si c'est ResponseBuilder qui stocke quelque part dans la class Conversation
//a voir comment ca s'articule direct avec le dispatcher
// le dispatcher peut en tout cas appeler directement dans ce cas la..
