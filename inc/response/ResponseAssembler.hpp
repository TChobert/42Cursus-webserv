#pragma once

# include "webserv.hpp"

class ResponseAssembler
{
	public:
		static std::string assemble(const std::string& statusLine, const std::string& headers, const std::string& body, int status);
};
