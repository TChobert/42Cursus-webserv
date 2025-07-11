#include "../../inc/webserv.hpp"
#include <exception>
#include <stdexcept>
#include <algorithm>

using namespace std;
typedef map<string, locationConfig>::iterator locIt;

void Validator::execute(Conversation& conv) {
	validateCritical(conv);
	if (conv.state != VALIDATE)
		return;
	validateBenign(conv);
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
		if (conv.req.bodyLeft > bodyMax)
			return earlyResponse(conv, ENTITY_TOO_LARGE);
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

void Validator::validateUri(Conversation& conv) {
	if (conv.req.uri[0] != '/')
		stripHost(conv);
	if (conv.state != VALIDATE)
		return;
	size_t best = matchLoc(conv);
	if (!best)
		skipBody(conv, NOT_FOUND);
}

void Validator::stripHost(Conversation& conv) {
	string& s = conv.req.uri;
	if (s.compare(0, 7,"http://"))
		return skipBody(conv, BAD_REQUEST);
	s.erase(0, 7);
	if (s.compare(0, conv.config.identity.host.size(), conv.config.identity.host))
		return skipBody(conv, BAD_REQUEST);
	s.erase(0, conv.config.identity.host.size());
	if (!s.size())
		s = "/";
	if (s[0] == '/')
		return;
	if (s[0] != ':')
		return skipBody(conv, BAD_REQUEST);
	s.erase(0, 1);
	size_t dgt = s.find_first_not_of(base10);
	if (dgt == npos)
		dgt = s.size();
	size_t port;
	try {
		port = extractSize(s);
	} catch (exception& e) {
		return skipBody(conv, BAD_REQUEST);
	}
	if (port != conv.config.identity.port)
		return skipBody(conv, BAD_REQUEST);
	s.erase(0, dgt);
	if (!s.size())
		s = "/";
	if (s[0] != '/')
		return skipBody(conv, BAD_REQUEST);
}

size_t Validator::matchLoc(Conversation& conv) {
	size_t res = 0;
	for (locIt it = conv.config.locations.begin(); it != conv.config.locations.end(); it++) {
		size_t curr = 0;
		while (curr < min(it->first.size(), conv.req.uri.size())
				&& it->first[curr] == conv.req.uri[curr])
			curr++;
		if (curr > res) {
			res = curr;
			conv.location = &it->second;
		}
	}
	return res;
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

void Validator::validateHeader(Conversation& conv) {}
