#include "CommentsRemover.hpp"
#include "ConfigFileReader.hpp"

int	main(int ac, char **av) {

	if (ac !=2) {
		return (EXIT_FAILURE);
	}

	ConfigFileReader reader;
	std::string rawContent = reader.loadConfigContent(av[1]);

	CommentsRemover remover;
	std::string clearContent = remover.remove(rawContent);

	std::cout << clearContent;
	return (EXIT_SUCCESS);
}
