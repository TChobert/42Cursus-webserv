#include "../../inc/parse/parse_utils.hpp"
#include <sstream>

using namespace std;

void parseThrow(string what) {
#ifdef PARSE_DEBUG
	cerr << what << endl;
#endif
	throw exception();
}

void toLower(string& s) {
	for (size_t i = 0; i < s.size(); i++)
		s[i] = tolower(s[i]);
}

int extractInt(string& s) {
	stringstream ss(s);
	int res;
	if (!(ss>>res) || res < 0)
		parseThrow("Bad Int");
	ss >> s;
	return res;
}

string extractToken(string& s) {
	int pos = s.find_first_not_of(tchar);
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

/*
bool isCTL(char c) {return c <= 31 || c == 127;}

bool isUnserved(char c) {return isalnum(c) || mark.find(c) != npos;}

*/
