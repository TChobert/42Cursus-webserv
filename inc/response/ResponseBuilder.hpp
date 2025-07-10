#ifndef RESPONSEBUILDER_HPP
# define RESPONSEBUILDER_HPP

# include <string>
# include "StatusLineBuilder.hpp"
# include "HeaderBuilder.hpp"
# include "ResponseAssembler.hpp"

class Conversation;

class ResponseBuilder
{
	private:
		static void	build(Conversation& conv);

	public:
		static void	execute(Conversation& conv);
};

#endif
