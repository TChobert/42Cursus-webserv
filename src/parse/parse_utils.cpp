#include "../../inc/parse/parse_utils.hpp"
#include <sstream>

using namespace std;

bool isCTL(char c) {return c <= 31 || c == 127;}
bool isUnserved(char c) {return isalnum(c) || mark.find(c) != npos;}

void parseThrow(string what) {
#ifdef PARSE_DEBUG
	cerr << what << endl;
#endif
	throw exception();
}

int extractInt(string& s) {
	stringstream ss(s);
	int res;
	if (!(ss>>res) || res < 0)
		parseThrow("Bad version");
	ss >> s;
	return res;
}

void deleteLWS(string& s) {
	while (!s.compare("\r\n ") || !s.compare("\r\n\t")
			|| s.compare(" ") || !s.compare("\t")) {
		if (!s.compare("\r\n"))
			s.erase(0, 2);
		s.erase(0, 1);
	}
}

string extractUntilLWS(string& s) {
	size_t posMin = s.size();
	size_t pos = s.find("\r\n");
	if (pos != npos)
		posMin = min(posMin, pos);
	pos = s.find(" ");
	if (pos != npos)
		posMin = min(posMin, pos);
	pos = s.find("\t");
	if (pos != npos)
		posMin = min(posMin, pos);
	string res = s.substr(0, pos);
	s.erase(0, pos);
	return res;
}

bool isLegalToken(const string& s) {
	for (char c : s)
		if (isCTL(c))
			return false;
	return s.size();
}

string extractToken(string& s) {
	int pos = s.find_first_of(separator);
	if (pos == npos)
		pos = s.size();
	string res = s.substr(0, pos);
	s.erase(0, pos);
	return res;
}
