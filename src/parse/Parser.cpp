#include "../../inc/parse/Parser.hpp"
#include <cassert>
#include <exception>
#include <iostream>

using namespace std;
using namespace ParserRoutine;

bool Parser::parseStartLine(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n");
	if (pos > startLineMax) {
		handleHugeStart(conv);
		return false;
	}
	conv.req.method = extractMethod(s);
	conv.req.uri = extractRequestUri(s);
	conv.req.version = extractHttpVersion(s);
	return true;
}

void Parser::handleHugeStart(Conversation& conv) {
	std::string res = extractMethod(conv.buf);
	if (res.size() > methodMax) {
		conv.resp.statusCode = NOT_IMPLEMENTED;
		conv.resp.shouldClose = true;
		conv.state = RESPONSE;
		return;
	}
	res = extractRequestUri(conv.buf);
	if (res.size() > uriMax) {
		conv.resp.statusCode = URI_TOO_LONG;
		conv.resp.shouldClose = true;
		conv.state = RESPONSE;
		return;
	}
	parseThrow("Bad start line");
}

bool Parser::parseHeader(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n\r\n");
	if (pos == npos) {
		if (s.size() > headerMax) {
			conv.resp.statusCode = REQUEST_HEADER_FIELDS_TOO_LARGE;
			conv.resp.shouldClose = true;
			conv.state = RESPONSE;
			return false;
		}
		conv.state = READ_CLIENT;
		return false;
	}
	conv.req.header = parseAllField(s);
	conv.state = VALIDATE;
	conv.pState = BODY;
	return true;
}

bool Parser::parseBodyChunked(Conversation& conv) {
	conv.resp.statusCode = NOT_IMPLEMENTED;
	conv.resp.shouldClose = true;
	conv.state = RESPONSE;
	return false;
	#ifdef PARSE_DEBUG
	std::cerr << "Chunked body not implemented\n";
	#endif
}

bool Parser::parseBody(Conversation& conv) {
	if (conv.req.header.count("content-length")) {
		size_t pos = min(conv.buf.size(), conv.bodyLeft);
		conv.req.body += conv.buf.substr(0, pos);
		conv.bodyLeft -= pos;
		conv.buf.erase(0, pos);
		if (conv.bodyLeft) {
			conv.state = READ_CLIENT;
			return false;
		}
		conv.buf.erase(0, 2);
		conv.state = EXEC;
		conv.pState = HEADER;
		return false;
	} else {
		return (parseBodyChunked(conv));
	};
}

void Parser::parse(Conversation& conv) {
	try {
		if (conv.pState == BODY && conv.state == PARSE_HEADER)
			conv.pState = SKIP_BODY;
		switch (conv.pState) {
			case SKIP_BODY:
				if (!parseBody(conv))
					break;
			case START:
				if (!parseStartLine(conv))
					break;
			case HEADER:
				parseHeader(conv);
				break;
			case BODY:
				if (!parseBody(conv))
					break;
		}
	} catch (std::exception& e) {
		conv.state = RESPONSE;
		conv.resp.statusCode = BAD_REQUEST;
		conv.resp.shouldClose = true;
	}
}
