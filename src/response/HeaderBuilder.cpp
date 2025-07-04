#include "HeaderBuilder.hpp"
#include "utils.hpp"

/* ---------------- PUBLIC METHODS ------------------ */

std::string HeaderBuilder::build(const HttpResponse& response, const HttpRequest& request)
{
	std::string headers;

	headers += HeaderBuilder::buildGenericHeaders(response, request);
	headers += HeaderBuilder::buildCustomHeaders(response);
	headers += "\r\n";

	return headers;
}

/* ---------------- PRIVATE METHODS ------------------ */

std::string HeaderBuilder::buildGenericHeaders(const HttpResponse& response, const HttpRequest& request)
{
	std::string headers;

	headers += "Date: " + getCurrentHttpDate() + "\r\n";

	return headers;
}

std::string HeaderBuilder::buildCustomHeaders(const HttpResponse& response)
{

}
