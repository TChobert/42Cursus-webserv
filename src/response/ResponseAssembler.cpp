#include "ResponseAssembler.hpp"

std::string ResponseAssembler::assemble(const std::string& statusLine, const std::string& headers, const std::string& body)
{
	return statusLine + headers + "\r\n" + body;
}
