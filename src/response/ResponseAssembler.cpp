#include "ResponseAssembler.hpp"
#include "HeaderBuilder.hpp"

std::string ResponseAssembler::assemble(const std::string& statusLine, const std::string& headers, const std::string& body, int status)
{
	if (!HeaderBuilder::isBodyForbidden(status) && !body.empty())
		return statusLine + headers + "\r\n" + body;
	else
		return statusLine + headers + "\r\n";
}
