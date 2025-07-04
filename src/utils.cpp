#include "utils.hpp"

#include <sstream>
#include <ctime>
#include <string>

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
