#include "ResponseBuilder.hpp"

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.response);
	std::string headers = HeaderBuilder::build(conv.response);
	std::string body = BodyBuilder::build(conv.response);

	conv.final_response = ResponseAssembler::assemble(statusLine, headers, body);

	//MAJ de l'etat
	conv.state = Conversation::WRITE_CLIENT; // tout depend si l'enum est a l'interieur ou exterieur de Conv...
}

//a voir si on return direct ou si c'est ResponseBuilder qui stocke quelque part dans la class Conversation
//a voir comment ca s'articule direct avec le dispatcher
// le dispatcher peut en tout cas appeler directement dans ce cas la..
