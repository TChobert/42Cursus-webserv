#include <iostream>
#include "ServerSectionParser.hpp"
#include "ServerDirectivesProcessor.hpp"

int main() {
    ServerSectionParser serverParser;
    ServerDirectivesProcessor directivesProcessor;

    parserContext *context = NULL;

    try {
        std::cout << "=== Test ServerSectionParser ===" << std::endl;

        std::string directives[] = {
            "port=8080",
            "host=127.0.0.1",
            "name=my_server",
            "root=/var/www"
        };

        for (size_t i = 0; i < sizeof(directives)/sizeof(directives[0]); ++i) {
            std::cout << "Processing directive: " << directives[i] << std::endl;
            serverParser.extractCurrentDirective(directives[i], context);
        }

        std::cout << "\n=== Test ServerDirectivesProcessor ===" << std::endl;

        directivesProcessor.processPortDirective("8080", context);
        directivesProcessor.processHostDirective("localhost", context);
        directivesProcessor.processNameDirective("test_server", context);
        directivesProcessor.processRootDirective("/tmp", context);

        std::cout << "Tous les tests ont réussi !" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Erreur attrapée : " << e.what() << std::endl;
    }

    return 0;
}
