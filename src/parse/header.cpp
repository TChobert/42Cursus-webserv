#include "../../inc/parse/parse.hpp"

using namespace std;
using namespace ParserRoutine;

mapStr parseAllField(string& s) {
	mapStr res;
	while (s != "") {
		pairStr h = extractOneField(s);
		if (res.count(h.first)) {
			mapStr::iterator it = res.find(h.first);
			it->second += ", ";
			it->second += h.second;
		} else
			res[h.first] = h.second;
	}
	return res;
}

pairStr extractOneField(string& s) {
	pairStr res;
	res.first = extractToken(s);
	toLower(res.first);
	if (res.first.empty() || s[0] != ':')
		parseThrow("Bad field-name");
	size_t pos = s.find("\r\n");
	if (pos == npos)
		parseThrow("Bad field-value");
	res.second = s.substr(0, pos);
	deleteLeadOWS(res.second);
	deleteTrailOWS(res.second);
	if (!isValidFieldValue(res.second))
		parseThrow("Bad field-value");
	s.erase(0, pos + 2);

	return res;
}

bool isValidFieldValue(string& s) {
	for (size_t i = 0; i < s.size(); i++) {
		if ((s[i] >= 0 && s[i] < ' ' && s[i] != '\t')
				|| s[i] == 127)
			return false;
	}
	return true;
}
