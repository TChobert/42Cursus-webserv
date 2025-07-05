#ifndef RESPONSEASSEMBLER_HPP
# define RESPONSEASSEMBLER_HPP

# include "HttpResponse.hpp"

class ResponseAssembler
{
	public:
		static std::string assemble(const std::string& statusLine, const std::string& headers, const std::string& body)
};

#endif

