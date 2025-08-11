#include "../../inc/webserv.hpp"
#include "parse/parse_utils.hpp"
#include "validate/validate.hpp"
#include "webserv_enum.hpp"
#include "webserv_utils.hpp"
#include <stdexcept>
#include <algorithm>

using namespace std;

void Validator::execute(Conversation& conv) {
	validateCritical(conv);
	if (conv.state != VALIDATE)
		return;
	validateBenign(conv);
	if (conv.state == VALIDATE) {
		if (conv.req.bodyLeft || conv.req.header.count("transfer-encoding"))
			conv.state = PARSE_BODY;
		else
			conv.state = EXEC;
	}
}

void Validator::validateCritical(Conversation& conv) {
	validateVersion(conv);
	validateBodyFormat(conv);
}

void Validator::validateVersion(Conversation& conv) {
	if (conv.req.version != make_pair(1, 1))
		earlyResponse(conv, HTTP_VERSION_NOT_SUPPORTED);
}


void Validator::validateBodyFormat(Conversation& conv) {
	if (conv.req.header.count("content-length")
			&& conv.req.header.count("transfer-encoding"))
		return earlyResponse(conv, BAD_REQUEST);
	conv.req.bodyLeft = 0;

	if (conv.req.header.count("content-length")) {
		try {
			conv.req.bodyLeft = peekSize(conv.req.header["content-length"]);
		} catch (std::overflow_error& e) {
			return earlyResponse(conv, ENTITY_TOO_LARGE);
		} catch (std::invalid_argument& e) {
			return earlyResponse(conv, BAD_REQUEST);
		}
		if (conv.location && conv.req.bodyLeft > conv.location->clientMaxBodySize)
		{
			std::cout << "CONV.BODYLEFT = " << conv.req.bodyLeft << std::endl;
			std::cout << "BODYMAX = " << conv.location->clientMaxBodySize << std::endl;
			return earlyResponse(conv, ENTITY_TOO_LARGE);
		}
		if (conv.req.header["content-length"].find_first_not_of(base10) != npos)
			return earlyResponse(conv, BAD_REQUEST);

	} else if (conv.req.header.count("transfer-encoding")) {
		if (conv.req.header["transfer-encoding"] != "chunked")
			return earlyResponse(conv, NOT_IMPLEMENTED);
	}
}

void Validator::validateBenign(Conversation& conv) {
	validateUri(conv);
	if (conv.state != VALIDATE)
		return;
	validateMethod(conv);
	if (conv.state != VALIDATE)
		return;
	validateHeader(conv);
	if (conv.state != VALIDATE)
		return;
}
void Validator::validateMethod(Conversation& conv) {
	if (conv.req.method != "GET" && conv.req.method != "POST"
			&& conv.req.method != "DELETE") {
		skipBody(conv, NOT_IMPLEMENTED);
		return;
	}
	vector<string>& meth = conv.location->allowedMethods;
	if (find(meth.begin(), meth.end(), conv.req.method) == meth.end())
		skipBody(conv, METHOD_NOT_ALLOWED);
}

// void Validator::validateHeader(Conversation& conv) {
// 	mapStr& head = conv.req.header;
// 	if (!head.count("host")
// 			|| (head["host"] != conv.config.identity.host
// 				&& head["host"] != conv.config.identity.host + ':' + intToString(conv.config.identity.port)))
// 		return earlyResponse(conv, BAD_REQUEST);
// 	if (head.count("content-encoding"))
// 		return skipBody(conv, NOT_IMPLEMENTED);
// 	if (head.count("connection")) {
// 		if (head["connection"] == "close") {
// 			conv.resp.shouldClose = true;
// 		} else if (head["connection"] != "keep-alive")
// 			return skipBody(conv, NOT_IMPLEMENTED);
// 	}
// 	//if (head.count("cookie")) {
// 	//	conv.req.cookie = head["cookie"];
// 	//}
// 	if (head.count("expect")) {
// 		if (head["expect"] == "100-continue") {
// 			if (!conv.resp.status) {
// 				conv.resp.status = CONTINUE;
// 				conv.state = RESPONSE;
// 			}
// 		} else
// 			return earlyResponse(conv, EXPECTATION_FAILED);
// 	}
// }

void Validator::validateHeader(Conversation& conv) {
	mapStr& head = conv.req.header;

	if (!head.count("host")) {
		return earlyResponse(conv, BAD_REQUEST);
	}

	std::string hostHeader = head["host"];
	std::string expectedHost = conv.config.identity.host; // e.g. "0.0.0.0"
	std::string expectedName = conv.config.identity.serverName; // e.g. "test_server"
	std::string expectedPortStr = intToString(conv.config.identity.port);

	// Split Host header: host[:port]
	std::string hostOnly = hostHeader;
	std::string portOnly = "";
	size_t colon = hostHeader.find(':');
	if (colon != std::string::npos) {
		hostOnly = hostHeader.substr(0, colon);
		portOnly = hostHeader.substr(colon + 1);
	}

	// Validate port (if specified)
	if (!portOnly.empty() && portOnly != expectedPortStr) {
		std::cout << "[DEBUG] Port mismatch: got " << portOnly << ", expected " << expectedPortStr << std::endl;
		return earlyResponse(conv, BAD_REQUEST);
	}

	// Check if host matches any expected value
	bool hostMatch = false;

	if (expectedHost == "0.0.0.0") {
		// Accept any local connection
		hostMatch = true;
	} else if (hostOnly == expectedHost || hostOnly == expectedName) {
		hostMatch = true;
	} else if ((expectedHost == "127.0.0.1" && hostOnly == "localhost") ||
	           (expectedHost == "localhost" && hostOnly == "127.0.0.1")) {
		hostMatch = true;
	}

	if (!hostMatch) {
		std::cout << "[DEBUG] Host mismatch: got '" << hostOnly << "', expected '" << expectedHost << "' or '" << expectedName << "'" << std::endl;
		return earlyResponse(conv, BAD_REQUEST);
	}

	// Handle remaining headers
	if (head.count("content-encoding")) {
		return skipBody(conv, NOT_IMPLEMENTED);
	}

	if (head.count("connection")) {
		if (head["connection"] == "close") {
			conv.resp.shouldClose = true;
		} else if (head["connection"] != "keep-alive") {
			return skipBody(conv, NOT_IMPLEMENTED);
		}
	}

	if (head.count("expect")) {
		if (head["expect"] == "100-continue") {
			if (!conv.resp.status) {
				conv.resp.status = CONTINUE;
				conv.state = RESPONSE;
			}
		} else {
			return earlyResponse(conv, EXPECTATION_FAILED);
		}
	}
}
