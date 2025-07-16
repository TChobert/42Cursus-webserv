#include "Formatter.hpp"

Formatter::Formatter(void) {}

Formatter::~Formatter(void) {}

bool Formatter::isEmptyLine(const std::string& line) const {

	for (size_t i = 0; i < line.size(); ++i) {
		if (!isspace(static_cast<unsigned char>(line[i])))
			return (false);
	}
	return (true);
}

std::string Formatter::removeLeadingSpaces(const std::string& content) {

	std::stringstream rawContent(content);
	std::string result;
	std::string currentLine;

	while (getline(rawContent, currentLine)) {
		size_t firstNonSpace = 0;
		while (firstNonSpace < currentLine.size() && isspace(static_cast<unsigned char>(currentLine[firstNonSpace]))) {
			++firstNonSpace;
		}
		result += currentLine.substr(firstNonSpace) + '\n';
	}
	return (result);
}

std::string Formatter::removeEmptyLines(const std::string& content) {

	std::stringstream rawContent(content);
	std::string result;
	std::string currentLine;

	while (getline(rawContent, currentLine)) {

		if (isEmptyLine(currentLine) == false) {
			result += currentLine + '\n';
		}
	}
	return (result);
}

std::vector<std::string> Formatter::splitContent(const std::string& content) {

	std::vector<std::string> splittedContent;
	std::stringstream contentStream(content);
	std::string currentLine;

	while (getline(contentStream, currentLine)) {
		splittedContent.push_back(currentLine);
	}
	return (splittedContent);
}

std::vector<std::string> Formatter::format(const std::string& content) {

	std::vector<std::string> formatted;
	
	std::string leadingSpacesRemoved = removeLeadingSpaces(content);
	std::string emptyLinesRemoved = removeEmptyLines(leadingSpacesRemoved);
	formatted = splitContent(emptyLinesRemoved);

	return (formatted);
}
