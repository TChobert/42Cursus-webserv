#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.resp);
	std::string headers = HeaderBuilder::build(conv);
	std::string body = conv.resp.body;

	conv.finalResponse = ResponseAssembler::assemble(statusLine, headers, body);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	ResponseBuilder::build(conv);
	conv.state = WRITE_CLIENT;
}

