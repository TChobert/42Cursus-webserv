#include "HeaderSectionParser.hpp"
#include <assert.h>
#include <cstdlib>
#include "ConfigParser.hpp"

int main(void) {

	HeaderSectionParser parser;
	parserContext context;

	try {
		HeaderSectionParser parser;
		parser.ensureHeaderIsEnclosed("[SERVER]");
		std::cout << "Header enclosed correctly.\n";
	} 
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	std::string header = "[SERVER]";
	std::cout << parser.trimHeader(header);
	try {
		parser.ensureHeaderIsEnclosed(header);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	assert(parser.getHeaderType("SERVER") == 0);
	parser.handleCurrentHeader(header, &context);

	assert(context.state == SERVER_SECTION);
	assert(context.isInServerScope == true);

	return (EXIT_SUCCESS);
}
