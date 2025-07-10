#include "ResponseBuilder.hpp"
#include "Conversation.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.response);
	std::string headers = HeaderBuilder::build(conv.response);
	std::string body = conv.response.body;

	conv.final_response = ResponseAssembler::assemble(statusLine, headers, body);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	ResponseBuilder::build(conv);
	conv.state = WRITE_CLIENT;
}

