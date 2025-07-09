#include "ResponseBuilder.hpp"
#include "Conversation.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	if (conv.statusCode >= 400)
	{
		if (conv.response.contentType.empty())
			conv.response.contentType = "text/plain"; // Peut être du html en fonction erreur.. a voir !

		conv.response.shouldClose = true;
	}

	std::string statusLine = StatusLineBuilder::build(conv.response);
	std::string headers = HeaderBuilder::build(conv.response);
	std::string body = BodyBuilder::build(conv.response);

	conv.final_response = ResponseAssembler::assemble(statusLine, headers, body);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	ResponseBuilder::build(conv);
	conv.state = WRITE_CLIENT;
}

