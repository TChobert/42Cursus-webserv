#pragma once

#include <exception>
#include <iostream>
#include <string>

class ServerDirectivesProcessor {

	private:

	enum keyType{PORT, HOST, NAME, ROOT, UNKNOWN};

	keyType getKeyType(const std::string& directive);

	public:

	typedef void (ServerDirectivesProcessor::*ProcessPtr)(const std::string&);
	ServerDirectivesProcessor(void);
	~ServerDirectivesProcessor(void);

	void processPortDirective(const std::string& directive);
	void processHostDirective(const std::string& directive);
	void processNameDirective(const std::string& directive);
	void processRootDirective(const std::string& directive);

	ProcessPtr getDirectiveProcess(const std::string& directive);
	class InvalidDirectiveException : public std::exception {
	public:
		virtual const char *what() const throw();
	};
};
