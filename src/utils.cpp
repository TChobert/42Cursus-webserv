#include "utils.hpp"

#include <sstream>

std::string intToString(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}
