#pragma once

#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>

class ConfigFileReader {

	public:

	ConfigFileReader(void);
	~ConfigFileReader(void);

	std::string	loadConfigContent(const std::string& configPath) const;

	class OpenFailureException: public std::exception {
		virtual const char *what() const throw();
	};

	class EmptyContentException: public std::exception {
		virtual const char *what() const throw();
	};
};
