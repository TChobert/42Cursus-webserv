#pragma once
#include <vector>
#include "Validator.hpp"

const std::string pchar = alpha + base10 + "-._~!$&'()*+,;=";
const std::string qchar = pchar + "?/";

void skipBody(Conversation& conv, statusCode status);
