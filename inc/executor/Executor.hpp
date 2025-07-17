#pragma once

#include "IModule.hpp"
#include "webserv_enum.hpp"
#include "webserv.hpp"
#include "ResourceChecker.hpp"
#include "CGIHandler.hpp"
#include "StaticFileHandler.hpp"

class Executor : public IModule
{
	private:
		void	handleGet(Conversation& conv);
		void	handlePost(Conversation& conv);
		void	handleDelete(Conversation& conv);

		void	handleFile(Conversation& conv);
		void	handleDirectory(Conversation& conv);

	public:
		void	execute(Conversation& conv);
};
