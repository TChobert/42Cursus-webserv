#include "Dispatcher.hpp"

Dispatcher::Dispatcher(int& EpollFd, IModule* reader, IModule* parser, IModule* validator, IModule* executor,
		IModule* responseBuilder, IModule* sender, IModule* postSender) :
		_epollFd(EpollFd), _reader(reader), _parser(parser), _validator(validator), _executor(executor),
		_responseBuilder(responseBuilder), _sender(sender), _postSender(postSender) {}

Dispatcher::~Dispatcher(void) {}

void	Dispatcher::setEpollInterest(int clientFd, e_interest_mode mode) {

	struct epoll_event	ev;

	ev.data.fd = clientFd;
	switch (mode) {
		case READ:
			ev.events = EPOLLIN;
			break;
		case WRITE:
			ev.events = EPOLLOUT;
			break;
	}
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &ev) < 0) {
		std::cerr << "Failed to change interest mode on fd: " << clientFd << std::endl;
	}
}

void	Dispatcher::removeClientFromEpoll(int clientFd) {

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL) < 0) {
		std::cerr << "Failed to remove fd " << clientFd << " from epoll\n";
	}
}

void	Dispatcher::dispatch(Conversation& conv) {

	while (true) {

		if (conv.state == RESPONSE) {
			_responseBuilder->execute(conv);
			setEpollInterest(conv.fd, WRITE);
			break ;
		}
		else if (conv.state == READ_CLIENT) {
			setEpollInterest(conv.fd, READ);
			break ;
		}
		else if (conv.state == WRITE_CLIENT) {
			_sender->execute(conv);
		}
		else if (conv.state == IS_SENT) {
			_postSender->execute(conv);
		}
		else if (conv.state == DRAIN_BUFFER || PARSE_HEADER || PARSE_BODY || SKIP_BODY || EOF_CLIENT) {
			_parser->execute(conv);
		}
		else if (conv.state == VALIDATE) {
			_validator->execute(conv);
		}
		else if (conv.state == FINISH) {
			removeClientFromEpoll(conv.fd);
			break ;
		}
	}
}
