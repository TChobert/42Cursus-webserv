#include "ConfigParser.hpp"

int main(int ac, char **av) {

	if (ac != 2)
		return (EXIT_FAILURE);

	std::string config = av[1];

	ConfigFileReader reader;
	HeaderSectionParser headerParser;
	ServerSectionParser serverParser;
	LocationSectionParser locParser;
	ConfigParser parser(config, reader, headerParser, serverParser, locParser);

	parser.parse();
	return (EXIT_SUCCESS);
}
