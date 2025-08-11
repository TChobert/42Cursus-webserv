#include "ResponseBuilder.hpp"

/* ---------------- PRIVATE ------------------ */

void	ResponseBuilder::build(Conversation& conv)
{
	std::string statusLine;
	std::string headers;
	std::string body;

	//----HEADERS----//
	if (!conv.headersSent && (conv.streamState == NORMAL || conv.streamState == START_STREAM))
	{
		statusLine = StatusLineBuilder::build(conv.resp);
		headers = HeaderBuilder::build(conv);
		std::cout << "RESPONSE BUILD: " << std::endl;
		std::cout << statusLine << headers << std::endl;
		conv.headersSent = true;
		if (conv.streamState == START_STREAM)
			conv.streamState = STREAM_IN_PROGRESS;
	}
	else
		headers.clear();

	//----BODY----//
	if (conv.streamState == START_STREAM || conv.streamState == STREAM_IN_PROGRESS)
	{
		// Chunked encoding
		if (!conv.resp.body.empty())
		{
			std::ostringstream chunk;
			chunk << std::hex << conv.resp.body.size() << "\r\n";
			chunk << conv.resp.body << "\r\n";
			body = chunk.str();
			conv.eState = WRITE_EXEC_POST_CGI;
			conv.state = WRITE_EXEC;
		}
		else if (conv.cgiFinished)
		{
			body = "0\r\n\r\n";
			conv.streamState = NORMAL;
			conv.eState = EXEC_START;
			conv.headersSent = false;
			conv.cgiFinished = false;
		}
	}
	else
		body = conv.resp.body;

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

