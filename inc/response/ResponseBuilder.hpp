#ifndef RESPONSEBUILDER_HPP
# define RESPONSEBUILDER_HPP

# include <string>
# include "StatusLineBuilder.hpp"
# include "HeaderBuilder.hpp"
# include "BodyBuilder.hpp"
# include "ResponseAssembler.hpp"

class Conversation;

class ResponseBuilder
{
	private:
		//je ne sais pas si je garde fonction buildError
		//je veux voir ce que ca donne par rapport a la personnalisation body
		//utile ou pas.. a voir (generer pages html, etc)
		static void	buildError(Conversation& conv);
		static void	build(Conversation& conv);

	public:
		static void	execute(Conversation& conv);
};

#endif
