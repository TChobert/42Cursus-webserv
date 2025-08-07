#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.resp);
	std::cout << "RESPONSE BUILD: " << std::endl;
	std::cout << statusLine << std::endl;
	std::string headers = HeaderBuilder::build(conv);
	std::cout << headers << std::endl;
	std::string body = conv.resp.body;

	conv.finalResponse = ResponseAssembler::assemble(statusLine, headers, conv.resp);
}

/* ---------------- PUBLIC ------------------ */

void	ResponseBuilder::execute(Conversation& conv)
{
	ResponseBuilder::build(conv);
	conv.state = WRITE_CLIENT;
}

void	ResponseBuilder::resetResponse(Conversation& conv)
{
	conv.finalResponse.clear();
	conv.bytesSent = 0;
	conv.resp = response();
}

