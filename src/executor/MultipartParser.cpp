#include "MultipartParser.hpp"
#include <sstream>
#include <cctype>

MultipartParser::MultipartParser(const std::string& body, const std::string& boundary)
	: _body(body), _boundary(boundary) {}

std::vector<std::string> MultipartParser::splitParts()
{
	std::vector<std::string> parts;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = _body.find(_boundary, start)) != std::string::npos)
	{
		if (end != start)
		{
			std::string part = _body.substr(start, end - start);
			if (!part.empty())
				parts.push_back(part);
		}
		start = end + _boundary.length();
	}
	return parts;
}

MultipartPart MultipartParser::parsePart(const std::string& part)
{
	MultipartPart parsed;
	std::string::size_type headerEnd = part.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return parsed;

	std::string headerSection = part.substr(0, headerEnd);
	std::string body = part.substr(headerEnd + 4);

	std::istringstream headers(headerSection);
	std::string line;
	while (std::getline(headers, line))
	{
		if (line.find("Content-Disposition:") != std::string::npos)
		{
			std::string::size_type namePos = line.find("name=\"");
			if (namePos != std::string::npos)
			{
				std::string::size_type end = line.find("\"", namePos + 6);
				parsed.name = line.substr(namePos + 6, end - (namePos + 6));
			}
			std::string::size_type filePos = line.find("filename=\"");
			if (filePos != std::string::npos)
			{
				std::string::size_type end = line.find("\"", filePos + 10);
				parsed.filename = line.substr(filePos + 10, end - (filePos + 10));
			}
		}
		else if (line.find("Content-Type:") != std::string::npos)
		{
			parsed.contentType = line.substr(14);
			if (!parsed.contentType.empty() && parsed.contentType[parsed.contentType.size() - 1] == '\r')
			{
				parsed.contentType.erase(parsed.contentType.size() - 1);
			}
		}
	}

	// Trim trailing CRLF or dashes
	while (!body.empty() && (body[body.size() - 1] == '\r' ||
							body[body.size() - 1] == '\n' ||
							body[body.size() - 1] == '-'))
	{
		body.erase(body.size() - 1);
	}

	parsed.data = body;
	return parsed;
}

std::vector<MultipartPart> MultipartParser::parse()
{
	std::vector<MultipartPart> result;
	std::vector<std::string> rawParts = splitParts();

	for (std::vector<std::string>::size_type i = 0; i < rawParts.size(); ++i)
	{
		MultipartPart p = parsePart(rawParts[i]);
		if (!p.name.empty())
			result.push_back(p);
	}
	return result;
}
