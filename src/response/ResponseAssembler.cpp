#include "ResponseAssembler.hpp"
#include "HeaderBuilder.hpp"

std::string ResponseAssembler::assemble(const std::string& statusLine, const std::string& headers, const response& resp)
{
	if (!HeaderBuilder::isBodyForbidden(resp.status) && !resp.body.empty())
		return statusLine + headers + "\r\n" + resp.body;
	else
		return statusLine + headers + "\r\n";
}
