// #include <iostream>
// #include <assert.h>
// #include <cstdlib>
// #include "ServerSectionParser.hpp"
// #include "ServerPropertiesProcessor.hpp"
// #include "ConfigParser.hpp"

// int main() {
//     ServerSectionParser serverParser;
//     ServerPropertiesProcessor propertiesProcessor;

//     parserContext context;

//     try {
//         std::cout << "=== Test ServerSectionParser ===" << std::endl;

//         std::string properties[] = {
//             "port=8080",
//             "host=127.0.0.1",
//             "name=my_server",
//             "root=/var/www"
//         };

//     	for (size_t i = 0; i < sizeof(properties)/sizeof(properties[0]); ++i) {
//             std::cout << "Processing property: " << properties[i] << std::endl;
//             serverParser.extractCurrentProperty(properties[i], &context);
//         }
//     }
// 	catch (const std::exception &e) {
//         std::cerr << "Erreur attrapée : " << e.what() << std::endl;
//     }

// 	assert(context.currentConfig.identity.port == 8080);
// 	assert(context.currentConfig.identity.host.compare("127.0.0.1") == 0);
// 	assert(context.currentConfig.identity.serverName.compare("my_server") == 0);
// 	assert(context.currentConfig.identity.root.compare("/var/www") == 0);
// 	std::cout << "Tous les tests ont réussi !" << std::endl;

// 	return (EXIT_SUCCESS);
// }

// #include <iostream>
// #include <assert.h>
// #include <cstdlib>
// #include "ServerSectionParser.hpp"
// #include "ServerPropertiesProcessor.hpp"
// #include "ConfigParser.hpp"

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "ServerSectionParser.hpp"
#include "ConfigParser.hpp" // Doit contenir ou inclure serverInfo

// ✅ Test valide
void runValidTestCase(const std::string& testName, const std::string props[], size_t size, const serverInfo& expected) {
	std::cout << "▶️ " << testName << std::endl;

	ServerSectionParser serverParser;
	parserContext context;

	try {
		for (size_t i = 0; i < size; ++i) {
			std::cout << "  - Processing: " << props[i] << std::endl;
			serverParser.extractCurrentProperty(props[i], &context);
		}
		assert(context.currentConfig.identity.port == expected.port);
		assert(context.currentConfig.identity.host == expected.host);
		assert(context.currentConfig.identity.serverName == expected.serverName);
		assert(context.currentConfig.identity.root == expected.root);
		assert(context.currentConfig.identity.hasRoot == expected.hasRoot);

		std::cout << "✅ Test passed!\n" << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "❌ Test failed with exception: " << e.what() << "\n" << std::endl;
	}
}

// ❌ Test invalide (doit échouer)
void runInvalidTestCase(const std::string& testName, const std::string props[], size_t size) {
	std::cout << "▶️ " << testName << std::endl;

	ServerSectionParser serverParser;
	parserContext context;

	try {
		for (size_t i = 0; i < size; ++i) {
			std::cout << "  - Processing: " << props[i] << std::endl;
			serverParser.extractCurrentProperty(props[i], &context);
		}
		std::cerr << "❌ Expected failure, but test passed.\n" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "✅ Correctly failed with exception: " << e.what() << "\n" << std::endl;
	}
}

int main() {
	std::cout << "=========== ServerPropertiesProcessor Test Suite ===========" << std::endl;

	// Round 1 — Valid input
	std::string validProps[] = {
		"port=8080",
		"host=127.0.0.1",
		"name=my_server",
		"root=/var/www"
	};
	serverInfo expected1 = {8080, "127.0.0.1", "my_server", "/var/www", true};
	runValidTestCase("Round 1: Valid config", validProps, 4, expected1);

	// Round 2 — Invalid ports
	std::string invalidPort1[] = {"port=abc", "host=localhost", "name=server1", "root=/var/www"};
	runInvalidTestCase("Round 2.1: Invalid port (non-integer)", invalidPort1, 4);

	std::string invalidPort2[] = {"port=99999", "host=localhost", "name=server1", "root=/var/www"};
	runInvalidTestCase("Round 2.2: Invalid port (out of range)", invalidPort2, 4);

	// Round 3 — Invalid host
	std::string invalidHost[] = {"port=8080", "host=", "name=server1", "root=/var/www"};
	runInvalidTestCase("Round 3: Invalid host (empty)", invalidHost, 4);

	// Round 4 — Invalid name (space)
	std::string invalidName[] = {"port=8080", "host=localhost", "name=invalid name", "root=/var/www"};
	runInvalidTestCase("Round 4: Invalid name (space)", invalidName, 4);

	// Round 5 — Invalid root (relative)
	std::string invalidRoot[] = {"port=8080", "host=localhost", "name=server1", "root=relative/path"};
	runInvalidTestCase("Round 5: Invalid root (not absolute)", invalidRoot, 4);

	// Round 6 — Edge cases (port 0 and 65535)
	std::string minPort[] = {"port=0", "host=localhost", "name=min", "root=/srv"};
	serverInfo expectedMin = {0, "localhost", "min", "/srv", true};
	runValidTestCase("Round 6.1: Port = 0", minPort, 4, expectedMin);

	std::string maxPort[] = {"port=65535", "host=localhost", "name=max", "root=/srv"};
	serverInfo expectedMax = {65535, "localhost", "max", "/srv", true};
	runValidTestCase("Round 6.2: Port = 65535", maxPort, 4, expectedMax);

	//Round7 - Duplicate port value

	std::string duplicatePort[] = {
    "port=8080",
    "port=9090",  // doublon
    "host=localhost",
    "name=server"
	};
	runInvalidTestCase("Round X: Duplicate port property", duplicatePort, 4);

	std::cout << "=========== All test rounds completed ===========" << std::endl;
	return EXIT_SUCCESS;
}
