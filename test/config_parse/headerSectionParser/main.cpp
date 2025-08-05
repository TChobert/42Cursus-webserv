// #include "HeaderSectionParser.hpp"
// #include <assert.h>
// #include <cstdlib>
// #include "ConfigParser.hpp"

// int main(void) {

// 	HeaderSectionParser parser;
// 	parserContext context;

// 	try {
// 		HeaderSectionParser parser;
// 		parser.ensureHeaderIsEnclosed("[SERVER]");
// 		std::cout << "Header enclosed correctly.\n";
// 	} 
// 	catch (const std::exception& e) {
// 		std::cerr << "Exception: " << e.what() << std::endl;
// 	}

// 	std::string header = "[SERVER]";
// 	std::cout << parser.trimHeader(header);
// 	try {
// 		parser.ensureHeaderIsEnclosed(header);
// 	}
// 	catch (const std::exception& e) {
// 		std::cerr << "Exception: " << e.what() << std::endl;
// 	}
// 	assert(parser.getHeaderType("SERVER") == 0);
// 	parser.handleCurrentHeader(header, &context);

// 	assert(context.state == SERVER_SECTION);
// 	assert(context.isInServerScope == true);

// 	return (EXIT_SUCCESS);
// }

#include "HeaderSectionParser.hpp"
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include "ConfigParser.hpp"

int main(void) {

    HeaderSectionParser parser;
    parserContext context;

    std::cout << "----- Test 1: ensureHeaderIsEnclosed with valid [SERVER] -----\n";
    try {
        parser.ensureHeaderIsEnclosed("[SERVER]");
        std::cout << "PASS: Header enclosed correctly.\n";
    } 
    catch (const std::exception& e) {
        std::cerr << "FAIL: Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----- Test 2: trimHeader(\"[SERVER]\") -----\n";
    std::string trimmed = parser.trimHeader("[SERVER]");
    std::cout << "Trimmed header: '" << trimmed << "'\n";
    assert(trimmed == "SERVER");

    std::cout << "----- Test 3: getHeaderType(\"SERVER\") -----\n";
    assert(parser.getHeaderType("SERVER") == 0); // SERVER enum = 0
    std::cout << "PASS: Header type is SERVER\n";

    std::cout << "----- Test 4: handleCurrentHeader with [SERVER]-----\n";
    parser.handleCurrentHeader("[SERVER]", &context);
    assert(context.state == SERVER_SECTION);
    assert(context.isInServerScope == true);
    std::cout << "PASS: Server section entered\n";

    std::cout << "----- Test 5: handleCurrentHeader with [LOCATION /path] -----\n";
    // simulate being already in server scope
    std::string locationHeader = "[LOCATION /path]";
    try {
        parser.handleCurrentHeader(locationHeader, &context);
        assert(context.state == LOCATION_SECTION);
        assert(context.currentLocationName == "/path");
        std::cout << "PASS: Location section handled\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: Exception in LOCATION header: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "----- Test 6: handleCurrentHeader with LOCATION before SERVER (should throw) -----\n";
    parserContext context2;
    context2.state = START;
    context2.isInServerScope = false;
    try {
        parser.handleCurrentHeader("[LOCATION /something]", &context2);
        std::cerr << "FAIL: Exception expected but not thrown\n";
        return EXIT_FAILURE;
    } catch (const HeaderSectionParser::ServerlessSectionException& e) {
        std::cout << "PASS: Caught expected ServerlessSectionException: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "FAIL: Unexpected exception type\n";
        return EXIT_FAILURE;
    }

    std::cout << "----- Test 7: ensureHeaderIsEnclosed with malformed header -----\n";
    try {
        parser.ensureHeaderIsEnclosed("SERVER]");
        std::cerr << "FAIL: Exception expected for missing '['\n";
        return EXIT_FAILURE;
    } catch (const HeaderSectionParser::InvalidHeaderException& e) {
        std::cout << "PASS: Caught expected InvalidHeaderException: " << e.what() << "\n";
    }

    try {
        parser.ensureHeaderIsEnclosed("[SERVER");
        std::cerr << "FAIL: Exception expected for missing ']'\n";
        return EXIT_FAILURE;
    } catch (const HeaderSectionParser::InvalidHeaderException& e) {
        std::cout << "PASS: Caught expected InvalidHeaderException: " << e.what() << "\n";
    }

    std::cout << "----- Test 8: trimHeader with spaces and tabs -----\n";
    std::string complexHeader = "[  LOCATION   /myPath   ]";
    std::string trimmedComplex = parser.trimHeader(complexHeader);
    std::cout << "Trimmed header: '" << trimmedComplex << "'\n";
    assert(trimmedComplex == "LOCATION   /myPath");

    std::cout << "----- Test 9: getHeaderType with invalid header -----\n";
    try {
        parser.getHeaderType("INVALID");
        std::cerr << "FAIL: Exception expected for invalid header type\n";
        return EXIT_FAILURE;
    } catch (const HeaderSectionParser::InvalidHeaderException& e) {
        std::cout << "PASS: Caught expected InvalidHeaderException for invalid header\n";
    }

    std::cout << "----- All tests passed! -----\n";

    return EXIT_SUCCESS;
}
