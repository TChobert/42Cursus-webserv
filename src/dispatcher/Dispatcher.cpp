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

void	Dispatcher::removeClientFromEpoll(Conversation& conv) {

	int	clientFd = conv.fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL) < 0) {
		std::cerr << "Failed to remove fd " << conv.config.identity.host << " from epoll\n";
	}
}

void	Dispatcher::dispatch(Conversation& conv) {

	if (conv.state == WRITE_CLIENT)
		conv.state = TO_SEND;
	else if (conv.state == READ_CLIENT)
		conv.state = TO_READ;
	else if (conv.state == READ_EXEC)
		conv.state = EXEC;
	//exec

	while (true) {

		if (conv.state == TO_SEND) {
			_sender->execute(conv);
		}
		else if (conv.state == EXEC) {
			_executor->execute(conv);
		}
		else if (conv.state == TO_READ) {
			_reader->execute(conv);
		}
		else if (conv.state == RESPONSE) {
			_responseBuilder->execute(conv);
		}
		else if (conv.state == IS_SENT) {
			_postSender->execute(conv);
		}
		else if (conv.state == PARSE || conv.state == PARSE_BODY || conv.state == EOF_CLIENT) {
			_parser->execute(conv);
		}
		else if (conv.state == VALIDATE) {
			_validator->execute(conv);
		}
		else if (conv.state == READ_EXEC) {
			setEpollInterest(conv.tempFd, READ_TEMP_FD);
			break ;
		}
		else if (conv.state == READ_CLIENT) {
			setEpollInterest(conv.fd, READ);
			break ;
		}
		else if (conv.state == WRITE_CLIENT) {
			setEpollInterest(conv.fd, WRITE);
			break ;
		}
		else if (conv.state == FINISH) {
			removeClientFromEpoll(conv);
			break ;
		}
	}
}
