#include <string>
#include "webserv.hpp"
#include "webserv_enum.hpp"

using namespace std;
typedef map<string, locationConfig>::iterator locIt;

void Validator::validateUri(Conversation& conv) {
	if (conv.req.uri[0] != '/')
		stripHost(conv);
	if (conv.state != VALIDATE)
		return;
	vector<string> seg = parsePath(conv);
	if (conv.state != VALIDATE)
		return;
	size_t match = matchLoc(conv, seg);
	if (!match)
		skipBody(conv, NOT_FOUND);
	assembleUri(conv, seg, match);
}

void Validator::stripHost(Conversation& conv) {
	string& s = conv.req.uri;
	toLower(s, 0, 6);
	if (s.compare(0, 6,"http:/"))
		return skipBody(conv, BAD_REQUEST);
	s.erase(0, 6);
	if (s.empty())
		s = "/";
	if (s[0] == '/')
		return;
	s.erase(0, 1);
	size_t pos = s.find("/");
	if (pos == npos)
		pos = s.size();
	toLower(s, 0, pos);
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

vector<string> Validator::parsePath(Conversation& conv) {
	string& s = conv.req.uri;
	vector<string> seg;
	while (s.size() && s[0] == '/') {
		while (s.size() && s[0] == '/')
			s.erase(0, 1);
		if (!s.size() || s[0] == '?')
			break;
		size_t pos = s.find_first_of("/?");
		if (pos == npos)
			pos = s.size();
		seg.push_back(s.substr(0, pos));
		s.erase(0, pos);
	}
	if (!seg.size())
		seg.push_back("");
	if (s.size()) {
		s.erase(0,1);
		conv.req.hasQuery = true;
		conv.req.query = s;
	}
	segmentLinter(conv, seg);
	if (conv.state != VALIDATE)
		return seg;
	queryLinter(conv);
	return seg;
}

void	segmentLinter(Conversation& conv, vector<string>& seg) {
	for (size_t i = 0; i < seg.size(); i++) {
		string decoded = "";
		for (size_t j = 0; j < seg[i].size();) {
			if (seg[i][j] == '%') {
				if (j + 2 >= seg[i].size() || base16.find(seg[i][j+1]) == npos
						|| base16.find(seg[i][j+2]) == npos)
					return earlyResponse(conv, BAD_REQUEST);
				string esc = seg[i].substr(j+1,2);
				decoded += peekSize(esc, 16);
				if (decoded[decoded.size()-1] == '/')
					return earlyResponse(conv, BAD_REQUEST);
				j+=2;
			} else {
				if (pchar.find(seg[i][j]) == npos)
					return earlyResponse(conv, BAD_REQUEST);
				decoded += seg[i][j];
				j++;
			}
		}
		seg[i] = decoded;
		if (seg[i] == "." || seg[i] == "..")
			return earlyResponse(conv, BAD_REQUEST);
	}
}

void	queryLinter(Conversation& conv) {
	string& quer = conv.req.query;
	for (size_t j = 0; j < quer.size();) {
		if (quer[j] == '%') {
			if (j + 2 >= quer.size() || base16.find(quer[j+1]) == npos
					|| base16.find(quer[j+2]) == npos)
				return earlyResponse(conv, BAD_REQUEST);
			j+=2;
		} else {
			if (qchar.find(quer[j]) == npos)
				return earlyResponse(conv, BAD_REQUEST);
			j++;
		}
	}
}

size_t Validator::matchLoc(Conversation& conv, vector<string>& seg) {
	size_t res = 0;
	for (locIt it = conv.config.locations.begin(); it != conv.config.locations.end(); it++) {
		size_t curr = 0;
		size_t pos = 0;
		while (pos < it->first.size()) {
			size_t newPos = it->first.find("/", pos+1);
			if (newPos == npos)
				newPos = it->first.size();
			if (seg[curr] == it->first.substr(pos+1, newPos-pos-1))
				curr++;
			else
				break;
			pos = newPos;
		}
		if (curr > res && pos == it->first.size()) {
			res = curr;
			conv.location = &it->second;
		}
	}
	return res;
}

void Validator::assembleUri(Conversation& conv, vector<string>& seg, size_t match) {
	if (conv.location->hasRedir) {
		conv.req.uri += conv.location->redirURL;
		conv.resp.status = conv.location->redirCode;
	}
	size_t i = 0;
	if (conv.location->hasRoot) {
		conv.req.uri += conv.location->root;
		i = match;
	}
	while (i < seg.size()) {
		conv.req.uri += '/' + seg[i];
		i++;
	}
}
