#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine = StatusLineBuilder::build(conv.resp);
	std::cout << "RESPONSE BUILD: " << std::endl;
	std::cout << statusLine << std::endl;

	//----HEADERS----//
	std::string headers;
	if (conv.streamState == NORMAL || conv.streamState == START_STREAM)
	{
		headers = HeaderBuilder::build(conv);
		if (conv.streamState == START_STREAM)
			conv.streamState = STREAM_IN_PROGRESS;
	}
	else
		headers.clear();
	std::cout << headers << std::endl;

	//----BODY----//
	std::string body;
	if (conv.streamState == STREAM_IN_PROGRESS || conv.streamState == START_STREAM)
	{
		// Chunked encoding
		if (!conv.resp.body.empty())
		{
			std::ostringstream chunk;
			chunk << std::hex << conv.resp.body.size() << "\r\n";
			chunk << conv.resp.body << "\r\n";
			body = chunk.str();
		}
		else if (conv.cgiFinished)
		{
			body = "0\r\n\r\n";
			conv.streamState = NORMAL;
			conv.cgiFinished = false;
		}
	}
	else
	{
		// Normal encoding
		body = conv.resp.body;
	}

	conv.finalResponse = ResponseAssembler::assemble(statusLine, headers, body, conv.resp.status);
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

