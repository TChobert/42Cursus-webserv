#pragma once

#include "IModule.hpp"

struct moduleRegistry {

	IModule *reader;
	IModule *parser;
	IModule *validator;
	IModule *executor;
	IModule *responseBuilder;
	IModule *sender;
	IModule *postSender;
	IModule *cgiExecutor;
};
