#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.resp);
	std::string headers = HeaderBuilder::build(conv);
	std::string body = conv.resp.body;

	conv.finalResponse = ResponseAssembler::assemble(statusLine, headers, conv.resp);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	ResponseBuilder::build(conv);
	std::cout << conv.finalResponse << std::endl;
	conv.state = WRITE_CLIENT;
}

void	ResponseBuilder::resetResponse(Conversation& conv)
{
	conv.finalResponse.clear();
	conv.bytesSent = 0;
	conv.resp = response();
}

