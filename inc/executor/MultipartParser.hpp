#pragma once

#include <string>
#include <vector>

struct MultipartPart
{
	std::string name;
	std::string filename;
	std::string contentType;
	std::string data;
};

class MultipartParser
{
	private:
		std::string _body;
		std::string _boundary;

		std::vector<std::string>	splitParts();
		MultipartPart				parsePart(const std::string& part);

	public:
		MultipartParser(const std::string& body, const std::string& boundary);
		std::vector<MultipartPart> parse();
};
