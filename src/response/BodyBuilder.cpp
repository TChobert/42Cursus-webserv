#include "BodyBuilder.hpp"

/* ---------------- PRIVATE METHODS ------------------ */

// std::string BodyBuilder::generateBodyError(int statusCode)
// {

// }

// >>> GENERER DES PAGES HTML, a inclure !!


/* ---------------- PUBLIC ------------------ */

std::string BodyBuilder::build(const HttpResponse& response)
{
	// if (response.statusCode >= 400 && response.body.empty())
	// {
	// 	return generateBodyError(response.statusCode);
	// }
	return response.body;
}




