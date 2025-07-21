#include <iostream>
#include "ServerSectionParser.hpp"
#include "ServerPropertiesProcessor.hpp"

int main() {
    ServerSectionParser serverParser;
    ServerPropertiesProcessor propertiesProcessor;

    parserContext *context = NULL;

    try {
        std::cout << "=== Test ServerSectionParser ===" << std::endl;

        std::string properties[] = {
            "port=8080",
            "host=127.0.0.1",
            "name=my_server",
            "root=/var/www"
        };

        for (size_t i = 0; i < sizeof(properties)/sizeof(properties[0]); ++i) {
            std::cout << "Processing property: " << properties[i] << std::endl;
            serverParser.extractCurrentProperty(properties[i], context);
        }

        std::cout << "\n=== Test ServerDirectivesProcessor ===" << std::endl;

        propertiesProcessor.processPortProperty("8080", context);
        propertiesProcessor.processHostProperty("localhost", context);
        propertiesProcessor.processNameProperty("test_server", context);
        propertiesProcessor.processRootProperty("/tmp", context);

        std::cout << "Tous les tests ont réussi !" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Erreur attrapée : " << e.what() << std::endl;
    }

    return 0;
}
