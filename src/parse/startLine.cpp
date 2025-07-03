#include "../../inc/parse/parse.hpp"

startLine parseStartLine(std::string& s) {
	startLine res;
	res.method = extractMethod(s);
	res.requestURI = extractRequestURI(s);
	res.method = extractMethod(s);
	return res;
}

std::string extractMethod(std::string& s) {
	std::string res = extractToken(s);
	if (!isLegalToken(res) || s[0] != ' ') {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad method\n";
		#endif
		throw std::exception();
	}
	s.erase(0, 1);
	return res;
}

// "authority is a valid URI so it shouldnt err, but it is hard to parse and
// useless because it is only valid for CONNECT that we will not implement..."
// Thus i would like to delay the linting of the URI to the validator
bool isValidUri(const std::string& s) {return true;}

std::string extractRequestURI(std::string& s) {
	int pos = s.find(' ');
	if (pos == npos) {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad URI\n";
		#endif
		throw std::exception();
	}
	std::string res = s.substr(0, pos);
	if (!isValidUri(res)) {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad URI\n";
		#endif
		throw std::exception();
	}
	s.erase(0, pos+1);
	return res;
}

std::pair<int, int> extractHTTPVersion(std::string& s) {
	if (s.compare(0, 5, "HTTP/")) {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad Version\n";
		#endif
		throw std::exception();
	}
	std::pair<int, int> res{0, 0};
	int i = 6;
	while (std::isdigit(s[i])) {
		res.first *= 10;
		res.first += s[i] - '0';
		i++;
	}
	if (s[i] != '.') {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad Version\n";
		#endif
		throw std::exception();
	}
	i++;
	while (std::isdigit(s[i])) {
		res.second *= 10;
		res.second += s[i] - '0';
		i++;
	}
	if (i != (int)s.size()) {
		#ifdef DEBUG_PARSE
		std::cerr << "Bad Version\n";
		#endif
		throw std::exception();
	}
	return res;
}
