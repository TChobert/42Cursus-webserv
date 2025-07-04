#include "../../inc/parse/parse.hpp"

using namespace std;
using namespace ParserRoutine;

mapStr parseAllHeader(string& s) {
	mapStr res;
	while (s != "") {
		pairStr h = extractOneHeader(s);
		if (res.count(h.first)) {
			mapStr::iterator it = res.find(h.first);
			it->second += ", ";
			it->second += h.second;
		} else
			res[h.first] = h.second;
	}
	return res;
}

//Not invalidating CTLs in field-content because
//they are allowed as part of quoted-pair inside
//quoted-string
pairStr extractOneHeader(string& s) {
	pairStr res;
	res.first = extractToken(s);
	if (!isLegalToken(s))
		parseThrow("Bad field-name");
	if (s[0] != ':')
		parseThrow("Bad header");
	while (s.size()) {
		deleteLWS(s);
		if (!s.compare("\r\n") || !s.size())
			break;
		if (res.second.size())
			res.second += ' ';
		res.second += extractUntilLWS(s);
	}
	if (!s.compare("\r\n"))
		s.erase(0, 2);
	return res;
}
