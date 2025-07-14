#include "ConfigFileReader.hpp"

int	main(int ac, char **av) {

	if (ac != 2)
		return (EXIT_FAILURE);

	try {
		ConfigFileReader reader;
		std::string result;
		result = reader.loadConfigContent(av[1]);
		std::cout << result;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return (EXIT_SUCCESS);
}
