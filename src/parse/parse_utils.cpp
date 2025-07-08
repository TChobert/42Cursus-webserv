#include <iostream>
#include <exception>
#include <string>
#include "../../inc/webserv.hpp"

using namespace std;

void parseThrow(string what) {
#ifdef PARSE_DEBUG
	cerr << what << endl;
#endif
	(void) what;
	throw exception();
}

void toLower(string& s) {
	for (size_t i = 0; i < s.size(); i++)
		s[i] = tolower(s[i]);
}

string extractToken(string& s) {
	size_t pos = s.find_first_not_of(tchar);
	if (pos == npos)
		pos = s.size();
	string res = s.substr(0, pos);
	s.erase(0, pos);
	return res;
}

void deleteLeadOWS(string& s) {
	size_t pos = s.find_first_not_of(" \t");
	if (pos == npos)
		pos = s.size();
	s.erase(0, pos);
}

void deleteTrailOWS(string& s) {
	size_t pos = s.find_last_not_of(" \t");
	if (pos == npos)
		return ;
	s.erase(pos + 1, s.size() - pos - 1);
}

void deleteChunkExt(string& s) {
	while (s.compare(0, 2, "\r\n")) {
		deleteLeadOWS(s);
		if (s[0] == ';') {
			s.erase(0, 1);
			deleteLeadOWS(s);
			string tok = extractToken(s);
			if (tok == "")
				parseThrow("Bad chunk extension");
			deleteLeadOWS(s);
			if (s[0] == '=') {
				s.erase(0, 1);
				deleteLeadOWS(s);
				if (s[0] == '"')
					deleteQuotedString(s);
				else {
					tok = extractToken(s);
					if (tok == "")
						parseThrow("Bad chunk extension");
				}
			}
		}
	}
	s.erase(0, 2);
}

void deleteQuotedString(string& s) {
	size_t i = 0;

	if (s[i] != '"')
		parseThrow("Bad quoted string");
	i++;
	while (i < s.size() && s[i] != '"') {
		if ((s[i] >= 0 && s[i] < ' ' && s[i] != '\t') || s[i] == 127)
			parseThrow("Bad quoted string");
		if (s[i] == '\\') {
			i++;
			if ((s[i] >= 0 && s[i] < ' ' && s[i] != '\t') || s[i] == 127)
			parseThrow("Bad quoted string");
		}
			i++;
	}
	if (s[i] != '"')
		parseThrow("Bad quoted string");
	s.erase(0, i+1);
}
