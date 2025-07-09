#include "../../inc/webserv.hpp"
#include <exception>
#include <stdexcept>

using namespace std;
using namespace ParserRoutine;

void Parser::parseStartLine(Conversation& conv) {
	string& s = conv.buf;
	while (!s.compare(0, 2, "\r\n"))
		s.erase(0, 2);
	size_t pos = s.find("\r\n");
	if (pos == npos && s.size() <= startLineMax)
		return;
	if ((pos != npos && pos > startLineMax)
			|| pos == npos) {
		handleHugeStart(conv);
		return;
	}
	conv.req.method = extractMethod(s);
	conv.req.uri = extractRequestUri(s);
	conv.req.version = extractHttpVersion(s);
	conv.pState = HEADER;
}

void Parser::handleHugeStart(Conversation& conv) {
	std::string res = extractMethod(conv.buf);
	if (res.size() > methodMax)
		return earlyResponse(conv, NOT_IMPLEMENTED);
	res = extractRequestUri(conv.buf);
	if (res.size() > uriMax)
		return earlyResponse(conv, URI_TOO_LONG);
	parseThrow("Bad start line");
}

void Parser::parseHeader(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n\r\n");
	if (pos == npos) {
		if (s.size() > headerMax)
			return earlyResponse(conv, REQUEST_HEADER_FIELDS_TOO_LARGE);
		conv.state = READ_CLIENT;
		return;
	}
	conv.req.header = parseAllField(s);
	conv.req.body = "";
	conv.state = VALIDATE;
	conv.pState = MAYBE_BODY;
	return;
}

void Parser::parseBody(Conversation& conv) {
	if (conv.req.header.count("content-length")) {
		size_t pos = min(conv.buf.size(), conv.req.bodyLeft);
		conv.req.body += conv.buf.substr(0, pos);
		conv.req.bodyLeft -= pos;
		conv.buf.erase(0, pos);
		if (conv.req.bodyLeft) {
			conv.state = READ_CLIENT;
			return;
		}
		conv.state = EXEC;
		conv.pState = START;
	} else {
		parseBodyChunked(conv);
	};
}

void Parser::parseBodyChunked(Conversation& conv) {
	string s = conv.buf;
	while (true) {
		size_t pos = s.find("\r\n");
		if (pos == npos && s.size() <= bodyMax)
			return;
		if ((pos != npos && pos > bodyMax) || pos == npos)
			return earlyResponse(conv, BAD_REQUEST);

		size_t chunkSize;
		try {
			chunkSize = peekSize(s, 16);
		} catch (std::overflow_error& e) {
			return earlyResponse(conv, CONTENT_TOO_LARGE);
		}

		if (s.size() - (pos + 2) < chunkSize + 2)
			return;
		extractSize(s, 16);
		deleteChunkExt(s);

		//Incorrect for trailers
		if (!chunkSize) {
			conv.pState = START;
			conv.state = EXEC;
			return;
		}
		if (conv.req.body.size() + chunkSize > bodyMax)
			return earlyResponse(conv, CONTENT_TOO_LARGE);
		conv.req.body += s.substr(0, chunkSize);
		s.erase(0, chunkSize);
		if (s.compare(0, 2, "\r\n"))
			parseThrow("Bad chunk");
		s.erase(0, 2);
	}
}

void Parser::execute(Conversation& conv) {
	try {
		if (conv.pState == MAYBE_BODY && conv.state == PARSE_HEADER)
			conv.pState = SKIP_BODY;
		if (conv.pState == MAYBE_BODY && conv.state == PARSE_BODY)
			conv.pState = BODY;
		if (conv.state == EOF_CLIENT) {
			if (conv.buf.empty() &&
					(conv.pState == START || conv.pState == MAYBE_BODY))
				conv.state = FINISH;
			else {
				return earlyResponse(conv, BAD_REQUEST);
			}
			return;
		}

		if (conv.pState == SKIP_BODY)
			parseBody(conv);
		if (conv.pState == START)
			parseStartLine(conv);
		if (conv.pState == HEADER)
			parseHeader(conv);
		if (conv.pState == BODY)
			parseBody(conv);
	} catch (std::exception& e) {
		return earlyResponse(conv, BAD_REQUEST);
	}
}
