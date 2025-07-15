#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <cctype>

class Formatter {

	private:

	bool isEmptyLine(const std::string& line) const;
	std::string	removeLeadingSpaces(const std::string& content);
	std::string	removeEmptyLines(const std::string& content);
	std::vector<std::string> splitContent(const std::string& content);

	public:

	Formatter(void);
	~Formatter(void);

	std::vector<std::string>	format(const std::string& content);
};
