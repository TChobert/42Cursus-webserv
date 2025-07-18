#pragma once

#include <exception>
#include <iostream>
#include <string>

class ServerSectionParser;
class ConfigParser;
struct parserContext;

class ServerDirectivesProcessor {

	private:

	enum keyType{PORT, HOST, NAME, ROOT, UNKNOWN};

	keyType getKeyType(const std::string& directiveKey);

	public:

	typedef void (ServerDirectivesProcessor::*ProcessPtr)(const std::string&, parserContext*);
	ServerDirectivesProcessor(void);
	~ServerDirectivesProcessor(void);

	void processPortDirective(const std::string& directive, parserContext *context);
	void processHostDirective(const std::string& directive, parserContext *context);
	void processNameDirective(const std::string& directive, parserContext *context);
	void processRootDirective(const std::string& directive, parserContext *context);

	ProcessPtr getDirectiveProcess(const std::string& directiveKey);
	class InvalidDirectiveException : public std::exception {
	public:
		virtual const char *what() const throw();
	};
};
