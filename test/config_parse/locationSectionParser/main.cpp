#include <cassert>
#include <iostream>
#include "LocationPropertiesProcessor.hpp"

class TestableLocationPropertiesProcessor : public LocationPropertiesProcessor {
public:
    using LocationPropertiesProcessor::getKeyType;
    using LocationPropertiesProcessor::keyType;
    using LocationPropertiesProcessor::ROOT;
    using LocationPropertiesProcessor::ALLOWED_METHODS;
    using LocationPropertiesProcessor::UPLOAD_AUTH;
    using LocationPropertiesProcessor::UPLOAD_DIR;
    using LocationPropertiesProcessor::AUTOINDEX;
    using LocationPropertiesProcessor::CGI;
    using LocationPropertiesProcessor::RETURN;
    using LocationPropertiesProcessor::BODY_SIZE;
    using LocationPropertiesProcessor::ERROR_PAGE;
    using LocationPropertiesProcessor::UNKNOWN;
};

void runTests() {
    TestableLocationPropertiesProcessor processor;

    assert(processor.getKeyType("root") == TestableLocationPropertiesProcessor::ROOT);
    assert(processor.getKeyType("allowed_methods") == TestableLocationPropertiesProcessor::ALLOWED_METHODS);
    assert(processor.getKeyType("upload_enabled") == TestableLocationPropertiesProcessor::UPLOAD_AUTH);
    assert(processor.getKeyType("upload_directory") == TestableLocationPropertiesProcessor::UPLOAD_DIR);
    assert(processor.getKeyType("autoindex") == TestableLocationPropertiesProcessor::AUTOINDEX);
    assert(processor.getKeyType("cgi") == TestableLocationPropertiesProcessor::CGI);
    assert(processor.getKeyType("return") == TestableLocationPropertiesProcessor::RETURN);
    assert(processor.getKeyType("client_max_body_size") == TestableLocationPropertiesProcessor::BODY_SIZE);
    assert(processor.getKeyType("error_page") == TestableLocationPropertiesProcessor::ERROR_PAGE);

    assert(processor.getKeyType("unknown_directive") == TestableLocationPropertiesProcessor::UNKNOWN);
    assert(processor.getKeyType("") == TestableLocationPropertiesProcessor::UNKNOWN);
	assert(processor.getKeyType("        ") == TestableLocationPropertiesProcessor::UNKNOWN);
	assert(processor.getKeyType("cclient_max_body_size") == TestableLocationPropertiesProcessor::UNKNOWN);
	assert(processor.getKeyType("allowed") == TestableLocationPropertiesProcessor::UNKNOWN);
	

    std::cout << "✅ Tous les tests sont passés !" << std::endl;
}

int main() {
    runTests();
    return 0;
}
