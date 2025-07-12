#pragma once
#include <vector>
#include "Validator.hpp"

const std::string pchar = alpha + base10 + "-._~!$&'()*+,;=";
const std::string qchar = pchar + "?/";

struct parsedPath {
	std::vector<std::string> seg;
	bool hasQuery;
	std::string query;
};
void skipBody(Conversation& conv, statusCode status);
