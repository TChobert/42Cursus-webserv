#include "ResponseBuilder.hpp"
#include "Conversation.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::buildError(Conversation& conv)
{
	//a voir si ce sont les modules qui appellent direct des qu'il y a un probleme
	//ou est-ce que ca repasse par le dispatcher et les autres mettent uniquement statusCode = 500...
	//c'est cense repasser par le dispatcher... donc les autres modules doivent juste mettre le code a jour

	if (conv.response.contentType.empty())
		conv.response.contentType = "text/plain"; //ATTENTION: a etre modifie si ce sont des pages HTML
	conv.response.shouldClose = true;

	std::string statusLine = StatusLineBuilder::build(conv.response);
	std::string headers = HeaderBuilder::build(conv.response);
	std::string body = BodyBuilder::build(conv.response);

	conv.final_response = ResponseAssembler::assemble(statusLine, headers, body);
}

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.response);
	std::string headers = HeaderBuilder::build(conv.response);
	std::string body = BodyBuilder::build(conv.response);

	conv.final_response = ResponseAssembler::assemble(statusLine, headers, body);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	if (conv.statusCode >= 400)
		ResponseBuilder::buildError(conv);
	else
		ResponseBuilder::build(conv);
	conv.state = WRITE_CLIENT; // tout depend si l'enum est a l'interieur ou exterieur de Conv...
}

//a voir si on return direct ou si c'est ResponseBuilder qui stocke quelque part dans la class Conversation
//a voir comment ca s'articule direct avec le dispatcher
// le dispatcher peut en tout cas appeler directement dans ce cas la..

