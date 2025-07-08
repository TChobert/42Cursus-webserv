#include <stdexcept>
#include <string>
#include <sstream>
#include "../inc/webserv.hpp"

using namespace std;

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
	conv.state = RESPONSE;
	conv.resp.status = status;
	conv.resp.shouldClose = close;
}
