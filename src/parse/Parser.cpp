#include "../../inc/webserv.hpp"
#include <cassert>
#include <exception>
#include <iostream>

using namespace std;
using namespace ParserRoutine;

void Parser::parseStartLine(Conversation& conv) {
	string& s = conv.buf;
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

void Parser::parseHeader(Conversation& conv) {
	string& s = conv.buf;
	size_t pos = s.find("\r\n\r\n");
	if (pos == npos) {
		if (s.size() > headerMax) {
			conv.resp.statusCode = REQUEST_HEADER_FIELDS_TOO_LARGE;
			conv.resp.shouldClose = true;
			conv.state = RESPONSE;
			return;
		}
		conv.state = READ_CLIENT;
		return;
	}
	conv.req.header = parseAllField(s);
	conv.state = VALIDATE;
	conv.pState = MAYBE_BODY;
	return;
}

void Parser::parseBodyChunked(Conversation& conv) {
	conv.resp.statusCode = NOT_IMPLEMENTED;
	conv.resp.shouldClose = true;
	conv.state = RESPONSE;
	return;
	#ifdef PARSE_DEBUG
	std::cerr << "Chunked body not implemented\n";
	#endif
}

void Parser::parseBody(Conversation& conv) {
	if (conv.req.header.count("content-length")) {
		size_t pos = min(conv.buf.size(), conv.bodyLeft);
		conv.req.body += conv.buf.substr(0, pos);
		conv.bodyLeft -= pos;
		conv.buf.erase(0, pos);
		if (conv.bodyLeft) {
			conv.state = READ_CLIENT;
			return;
		}
		conv.state = EXEC;
		conv.pState = START;
	} else {
		parseBodyChunked(conv);
	};
}

void Parser::parse(Conversation& conv) {
	try {
		if (conv.pState == BODY && conv.state == PARSE_HEADER)
			conv.pState = SKIP_BODY;
		if (conv.pState == MAYBE_BODY && conv.state == PARSE_BODY)
			conv.pState = BODY;
		if (conv.state == EOF_CLIENT) {
			if (conv.buf.empty() &&
					(conv.pState == START || conv.pState == MAYBE_BODY))
				conv.state = FINISH;
			else {
				conv.state = RESPONSE;
				conv.resp.statusCode = BAD_REQUEST;
				conv.resp.shouldClose = true;
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
		conv.state = RESPONSE;
		conv.resp.statusCode = BAD_REQUEST;
		conv.resp.shouldClose = true;
	}
}
