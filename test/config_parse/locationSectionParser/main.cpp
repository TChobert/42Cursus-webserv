// #include <cassert>
// #include <iostream>
// #include "LocationPropertiesProcessor.hpp"

// class TestableLocationPropertiesProcessor : public LocationPropertiesProcessor {
// public:
//     using LocationPropertiesProcessor::getKeyType;
//     using LocationPropertiesProcessor::keyType;
//     using LocationPropertiesProcessor::ROOT;
//     using LocationPropertiesProcessor::ALLOWED_METHODS;
//     using LocationPropertiesProcessor::UPLOAD_AUTH;
//     using LocationPropertiesProcessor::UPLOAD_DIR;
//     using LocationPropertiesProcessor::AUTOINDEX;
//     using LocationPropertiesProcessor::CGI;
//     using LocationPropertiesProcessor::RETURN;
//     using LocationPropertiesProcessor::BODY_SIZE;
//     using LocationPropertiesProcessor::UNKNOWN;
// };

// void runTests() {
//     TestableLocationPropertiesProcessor processor;

//     assert(processor.getKeyType("root") == TestableLocationPropertiesProcessor::ROOT);
//     assert(processor.getKeyType("allowed_methods") == TestableLocationPropertiesProcessor::ALLOWED_METHODS);
//     assert(processor.getKeyType("upload_enabled") == TestableLocationPropertiesProcessor::UPLOAD_AUTH);
//     assert(processor.getKeyType("upload_directory") == TestableLocationPropertiesProcessor::UPLOAD_DIR);
//     assert(processor.getKeyType("autoindex") == TestableLocationPropertiesProcessor::AUTOINDEX);
//     assert(processor.getKeyType("cgi") == TestableLocationPropertiesProcessor::CGI);
//     assert(processor.getKeyType("return") == TestableLocationPropertiesProcessor::RETURN);
//     assert(processor.getKeyType("client_max_body_size") == TestableLocationPropertiesProcessor::BODY_SIZE);

//     assert(processor.getKeyType("unknown_directive") == TestableLocationPropertiesProcessor::UNKNOWN);
//     assert(processor.getKeyType("") == TestableLocationPropertiesProcessor::UNKNOWN);
// 	assert(processor.getKeyType("        ") == TestableLocationPropertiesProcessor::UNKNOWN);
// 	assert(processor.getKeyType("cclient_max_body_size") == TestableLocationPropertiesProcessor::UNKNOWN);
// 	assert(processor.getKeyType("allowed") == TestableLocationPropertiesProcessor::UNKNOWN);
	

//     std::cout << "✅ Tous les tests sont passés !" << std::endl;
// }

// int main() {
// 	runTests();
// 	return (EXIT_SUCCESS);
// }

#include <cassert>
#include <iostream>
#include "LocationPropertiesProcessor.hpp"
#include "LocationSectionParser.hpp"
#include "ConfigParser.hpp"

void testValidBodySizes() {
    LocationPropertiesProcessor processor;
    parserContext context;
    context.currentLocationName = "/";

    processor.fetchMaxBodySize("512", &context);
    assert(context.currentConfig.locations["/"].clientMaxBodySize == 512);
	context.seenLocationProperties.maxBodySeen = false;

    processor.fetchMaxBodySize("1K", &context);
    assert(context.currentConfig.locations["/"].clientMaxBodySize == 1024);
	context.seenLocationProperties.maxBodySeen = false;

    context.seenLocationProperties.maxBodySeen = false;
    processor.fetchMaxBodySize("2M", &context);
    assert(context.currentConfig.locations["/"].clientMaxBodySize == 2 * 1024 * 1024);
	context.seenLocationProperties.maxBodySeen = false;

    context.seenLocationProperties.maxBodySeen = false;
    processor.fetchMaxBodySize("1G", &context);
    assert(context.currentConfig.locations["/"].clientMaxBodySize == 1024 * 1024 * 1024);
	context.seenLocationProperties.maxBodySeen = false;
}

void testInvalidBodySizes() {
    LocationPropertiesProcessor processor;
    parserContext context;
    context.currentLocationName = "/";

    try {
        processor.fetchMaxBodySize("ABC", &context);
        assert(false && "Expected exception for non-numeric value");
    } catch (const std::exception&) {}

    try {
        processor.fetchMaxBodySize("999T", &context);
        assert(false && "Expected exception for unknown unit");
    } catch (const std::exception&) {}

    try {
        processor.fetchMaxBodySize("-100", &context);
        assert(false && "Expected exception for negative value");
    } catch (const std::exception&) {}
}

void testAutoIndexValues() {
    LocationPropertiesProcessor processor;
    parserContext context;
    context.currentLocationName = "/";

    processor.fetchAutoIndex("on", &context);
    assert(context.currentConfig.locations["/"].autoIndex == true);

    context.seenLocationProperties.autoIndexSeen = false;
    processor.fetchAutoIndex("off", &context);
    assert(context.currentConfig.locations["/"].autoIndex == false);

    try {
        context.seenLocationProperties.autoIndexSeen = false;
        processor.fetchAutoIndex("maybe", &context);
        assert(false && "Expected exception for invalid autoindex value");
    } catch (const std::exception&) {}
}

void testReturnDirective() {
    LocationPropertiesProcessor processor;
    parserContext context;
    context.currentLocationName = "/";

    processor.fetchLocationReturnInfo("301 http://example.com", &context);
    assert(context.currentConfig.locations["/"].hasRedir == true);
    assert(context.currentConfig.locations["/"].redirCode == MOVED_PERMANENTLY);
    assert(context.currentConfig.locations["/"].redirURL == "http://example.com");

    try {
        context.seenLocationProperties.returnSeen = false;
        processor.fetchLocationReturnInfo("400 invalid_url", &context);
        assert(false && "Expected exception for invalid return code");
    } catch (const std::exception&) {}
}

void testCgiDirective() {
    LocationPropertiesProcessor processor;
    parserContext context;
    context.currentLocationName = "/";

    // You should replace "/usr/bin/python3" by a valid executable path on your system
    std::string validHandler = "/usr/bin/python3";
    if (access(validHandler.c_str(), X_OK) != 0) {
        std::cout << "⚠️ Skipping CGI test: handler not found: " << validHandler << std::endl;
        return;
    }

    processor.processCgiProperty(".py:" + validHandler, &context);
    assert(context.currentConfig.locations["/"].cgiHandlers[".py"] == validHandler);

    try {
        context.seenLocationProperties.cgiSeen = false;
        processor.processCgiProperty(".py:/not/a/real/path", &context);
        assert(false && "Expected exception for invalid handler path");
    } catch (const std::exception&) {}
}

void runAllTests() {
    testValidBodySizes();
    testInvalidBodySizes();
    testAutoIndexValues();
    testReturnDirective();
    testCgiDirective();
    std::cout << "✅ Tous les tests supplémentaires sont passés !" << std::endl;
}

int main() {
    runAllTests();
    return 0;
}
