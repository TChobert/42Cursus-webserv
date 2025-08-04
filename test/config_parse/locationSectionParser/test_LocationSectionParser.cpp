#include <cassert>
#include <iostream>
#include "serverConfig.hpp"
#include "ConfigParser.hpp"
#include "LocationSectionParser.hpp"
#include "LocationPropertiesProcessor.hpp"

void testDefaultMethodsSetup() {
    parserContext context;
    context.currentLocationName = "/default";

    LocationSectionParser parser;
    parser.setupLocationDefaultMethods(&context);

    const std::vector<std::string>& methods = context.currentConfig.locations["/default"].allowedMethods;
    assert(methods.size() == 3);
    assert(methods[0] == "GET");
    assert(methods[1] == "POST");
    assert(methods[2] == "DELETE");
}

void testDefaultRootSetup_withServerRoot() {
    parserContext context;
    context.currentLocationName = "/";

    context.currentConfig.identity.hasRoot = true;
    context.currentConfig.identity.root = "/var/www/html";

    LocationSectionParser parser;
    parser.setupLocationDefaultRoot(&context);

    assert(context.currentConfig.locations["/"].root == "/var/www/html");
    assert(context.currentConfig.locations["/"].hasRoot == false);  // confirmé par le code
}

void testDefaultRootSetup_withoutServerRoot() {
    parserContext context;
    context.currentLocationName = "/";
    context.currentConfig.identity.hasRoot = false;

    LocationSectionParser parser;

    try {
        parser.setupLocationDefaultRoot(&context);
        assert(false && "Expected exception for rootless location");
    } catch (const LocationSectionParser::RootlessLocationException&) {
        // expected
    }
}

void testEnsureLocationConfigIsFull() {
    parserContext context;
    context.currentLocationName = "/";

    context.currentConfig.identity.hasRoot = true;
    context.currentConfig.identity.root = "/default_root";

    LocationSectionParser parser;
    parser.ensureLocationConfigIsFull(&context);

    const locationConfig& loc = context.currentConfig.locations["/"];
    assert(loc.clientMaxBodySize == DEFAULT_BODY_SIZE);
    assert(loc.uploadEnabled == false);
    assert(loc.autoIndex == false);
    assert(loc.hasRedir == false);
    assert(loc.allowedMethods.size() == 3);
    assert(loc.root == "/default_root");
}

void testExtractHeaderSwitchesState() {
    parserContext context;
    context.currentLocationName = "/";
    context.state = LOCATION_SECTION;

    context.currentConfig.identity.hasRoot = true;
    context.currentConfig.identity.root = "/default_root";

    LocationSectionParser parser;
    parser.extractCurrentProperty("[another_section]", &context);

    assert(context.state == HEADER_SECTION);
}

void runAllLocationSectionParserTests() {
    testDefaultMethodsSetup();
    testDefaultRootSetup_withServerRoot();
    testDefaultRootSetup_withoutServerRoot();
    testEnsureLocationConfigIsFull();
    testExtractHeaderSwitchesState();

    std::cout << "✅ Tous les tests LocationSectionParser sont passés !" << std::endl;
}

int main() {
    runAllLocationSectionParserTests();
    return 0;
}
