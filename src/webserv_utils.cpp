#include <stdexcept>
#include <string>
#include <sstream>
#include <ctime>

#include "../inc/webserv.hpp"

using namespace std;

void toLower(string& s, size_t start, size_t len) {
	for (size_t i = start; i < s.size() && i-start < len; i++)
		s[i] = tolower(s[i]);
}

//Calling with 1 param sets base to 10
size_t extractSize(string& s, int base) {
	size_t pos;
	if (base == 16)
		pos = s.find_first_not_of(base16);
	else
		pos = s.find_first_not_of(base10);
	if (pos == npos)
		pos = s.size();
	stringstream ss(s.substr(0,pos));
	s.erase(0, pos);
	size_t res;
	if (base == 16)
		ss >> std::hex;
	if (!(ss>>res)) {
		if (pos)
			throw std::overflow_error("Bad size_t");
		else
			throw std::invalid_argument("Bad size_t");
	}
	return res;
}

size_t peekSize(string& s, int base) {
	size_t pos;
	if (base == 16)
		pos = s.find_first_not_of(base16);
	else
		pos = s.find_first_not_of(base10);
	if (pos == npos)
		pos = s.size();
	stringstream ss(s.substr(0,pos));
	size_t res;
	if (base == 16)
		ss >> std::hex;
	if (!(ss>>res)) {
		if (pos)
			throw std::overflow_error("Bad size_t");
		else
			throw std::invalid_argument("Bad size_t");
	}
	return res;
}

//Calling with 2 params sets close to true
void earlyResponse(Conversation& conv, statusCode status, bool close) {
	conv.state = EXEC;
	conv.resp.status = status;
	conv.resp.shouldClose = close;
}

std::string intToString(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string getCurrentHttpDate()
{
	//Le format RFC 1123 donne des chaînes d’environ 29 à 35 caractères
	char buffer[80]; //strftime n'ecrit pas directement dans un std::string...
	std::time_t now = std::time(0);
	std::tm* gmt = std::gmtime(&now); //pour decomposer et mettre dans la struct tm

	// Format: "Fri, 04 Jul 2025 16:15:00 GMT"
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return std::string(buffer);
}
