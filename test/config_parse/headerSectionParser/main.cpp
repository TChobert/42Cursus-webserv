#include "HeaderSectionParser.hpp"
#include <assert.h>
#include <cstdlib>
#include "ConfigParser.hpp"

int main(void) {

	parserContext context;
	HeaderSectionParser parser;

	try {
    	HeaderSectionParser parser;
    	parser.ensureHeaderIsEnclosed("[SERVER]");
    	std::cout << "Header enclosed correctly.\n";
	} 
	catch (const std::exception& e) {
    	std::cerr << "Exception: " << e.what() << std::endl;
	}
}
	// std::string header = "[SERVER]";
	// parser.handleCurrentHeader(header, &context);

	// assert(context.state == HEADER_SECTION);
	// assert(context.isInServerScope == true);

	// return (EXIT_SUCCESS);
//}
