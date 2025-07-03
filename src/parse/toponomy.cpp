#include "../../inc/parse/parse.hpp"


bool isCTL(char c) {return c <= 31 || c == 127;}
bool isUnserved(char c) {return std::isalnum(c) || mark.find(c) != npos;}

bool isLegalToken(const std::string& s) {
	for (char c : s)
		if (isCTL(c))
			return false;
	return s.size();
}

std::string extractToken(std::string& s) {
	int pos = s.find_first_of(separator);
	if (pos == npos)
		pos = s.size();
	std::string res = s.substr(0, pos);
	s.erase(0, pos);
	return res;
}
