#include "../../inc/parse/Parser.hpp"
#include <cassert>
#include <exception>
#include <iostream>

using namespace std;
using namespace ParserRoutine;

void Parser::parseStartLine(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n");
	if (pos > startLineMax)
		return handleHugeStart(conv);
	conv.req.method = extractMethod(s);
	conv.req.uri = extractRequestUri(s);
	conv.req.version = extractHttpVersion(s);
}

void Parser::handleHugeStart(Conversation& conv) {
	std::string res = extractMethod(conv.buf);
	if (res.size() > methodMax) {
		conv.resp.statusCode = 501;
		conv.resp.shouldClose = true;
		conv.state = RESPONSE;
		return;
	}
	res = extractRequestUri(conv.buf);
	if (res.size() > uriMax) {
		conv.resp.statusCode = 414;
		conv.resp.shouldClose = true;
		conv.state = RESPONSE;
		return;
	}
	parseThrow("Bad start line");
}

void Parser::parseHeader(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n\r\n");
	if (pos == npos) {
		if (s.size() > headerMax) {
			conv.resp.statusCode = 431;
			conv.resp.shouldClose = true;
			conv.state = RESPONSE;
			return;
		}
		conv.state = READ_CLIENT;
		return;
	}
	parseStartLine(conv);
	if (conv.state == RESPONSE)
		return;
	conv.req.header = parseAllField(s);
	conv.state = VALIDATE;
	conv.pState = BODY;
}

void parseBodyChunked(Conversation& conv) {
	std::cerr << "Chunked body not implemented\n";
	assert(false);
}

void Parser::parseBody(Conversation& conv) {
	if (conv.req.header.count("content-length")) {
		size_t pos = min(conv.buf.size(), conv.bodyLeft);
		conv.req.body += conv.buf.substr(0, pos);
		conv.bodyLeft -= pos;
		if (conv.bodyLeft) {
			conv.state = READ_CLIENT;
		} else {
			conv.state = EXEC;
			conv.pState = HEADER;
		}
	} else 
		parseBodyChunked(conv);
}

void Parser::parse(Conversation& conv) {
	try {
		if (conv.pState == BODY && conv.state == PARSE_HEADER)
			conv.pState = SKIP_BODY;
		if (conv.pState == HEADER)
			parseHeader(conv);
		else if (conv.pState == BODY)
			parseBody(conv);
		else {
			parseBody(conv);
			if (conv.state == EXEC) {
				conv.pState = HEADER;
				parseHeader(conv);
			}
		}
	} catch (std::exception& e) {
		conv.state = RESPONSE;
		conv.resp.statusCode = 400;
		conv.resp.shouldClose = true;
	}
}
