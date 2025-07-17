#ifndef RESPONSEBUILDER_HPP
# define RESPONSEBUILDER_HPP

# include <string>
# include "StatusLineBuilder.hpp"
# include "HeaderBuilder.hpp"
# include "ResponseAssembler.hpp"
# include "IModule.hpp"

class Conversation;

class ResponseBuilder : public IModule
{
	private:
		static void	build(Conversation& conv);

	public:
		void		execute(Conversation& conv);
};

#endif
