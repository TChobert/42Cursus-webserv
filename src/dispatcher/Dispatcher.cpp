#include "Dispatcher.hpp"
#include <sys/stat.h>

Dispatcher::Dispatcher(int& EpollFd,
						std::map<int, Conversation>& clientsFds,
						std::map<int, Conversation*>& executorFds,
						IModule* reader,
						IModule* parser,
						IModule* validator,
						IModule* executor,
						IModule* responseBuilder,
						IModule* sender,
						IModule* postSender) :
	_epollFd(EpollFd),
	_clientsFds(clientsFds),
	_executorFds(executorFds),
	_reader(reader),
	_parser(parser),
	_validator(validator),
	_executor(executor),
	_responseBuilder(responseBuilder),
	_sender(sender),
	_postSender(postSender) {}

Dispatcher::~Dispatcher(void) {}

bool	Dispatcher::isInExecutorFdsMap(const int& fd) const {

	return _executorFds.find(fd) != _executorFds.end();
}

void	Dispatcher::setExecutorInterest(Conversation& conv, e_interest_mode mode) {

	if (conv.tempFd < 0) {
		std::cerr << "Invalid tempFd passed to setExecutorInterest" << std::endl;
		return ;
	}
	struct epoll_event ev;
	ev.data.fd = conv.tempFd;

	switch (mode) {
		case READ_EXEC_FD:
			ev.events = EPOLLIN;
			break;
		case WRITE_EXEC_FD:
			ev.events = EPOLLOUT;
			break;
	}
	if (!isInExecutorFdsMap(conv.tempFd)) {
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, conv.tempFd, &ev) < 0) {
			std::cerr << "Failed to add fd requested by executor (" << conv.tempFd << ") to interest list. Closing it." << std::endl;
			close(conv.tempFd);
			return ;
		}
		_executorFds[conv.tempFd]= &conv;
	}
	else {
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.tempFd, &ev) < 0) {
			std::cerr << "epoll_ctl MOD failed on fd " << conv.tempFd << ": " << strerror(errno) << ". Closing it." << std::endl;
			close(conv.tempFd);
			_executorFds.erase(conv.tempFd);
			return ;
		}
	}
}

void Dispatcher::setClientInterest(Conversation& conv, e_interest_mode mode) {
    std::cout << "setClientInterest called with fd: " << conv.fd << " mode: " << mode << std::endl;
    
    if (_clientsFds.find(conv.fd) == _clientsFds.end()) {
        std::cerr << "Client fd not found in clients map: " << conv.fd << std::endl;
        return ;
    }
    
    // VÉRIFIER SI LE SOCKET EST ENCORE VALIDE
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(conv.fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        std::cerr << "Socket fd " << conv.fd << " is invalid!" << std::endl;
        return;
    }
    if (error != 0) {
        std::cerr << "Socket fd " << conv.fd << " has error: " << strerror(error) << std::endl;
        return;
    }
    
    struct epoll_event ev;
    ev.data.fd = conv.fd;
    switch (mode) {
    case READ:
        ev.events = EPOLLIN;
        break;
    case WRITE:
        ev.events = EPOLLOUT;
        break;
    default:
        std::cerr << "Unknown mode in setClientInterest\n";
        return;
    }

	struct stat st;
	if (fstat(conv.fd, &st) == 0) {
		std::cout << "FD " << conv.fd << " is type: " << (st.st_mode & S_IFMT) << std::endl;
	}
	if (!S_ISSOCK(st.st_mode)) {
	std::cerr << "FD " << conv.fd << " is NOT a socket!" << std::endl;
	}
    if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.fd, &ev) < 0) {
        std::cerr << "epoll_ctl failed on fd: " << conv.fd 
                  << " mode: " << mode << " error: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Successfully changed fd " << conv.fd << " to mode " << mode << std::endl;
    }
}

// void Dispatcher::setClientInterest(Conversation& conv, e_interest_mode mode) {

// 	if (_clientsFds.find(conv.fd) == _clientsFds.end()) {
// 		std::cerr << "Client fd not found in clients map: " << conv.fd << std::endl;
// 		return ;
// 	}
// 	struct epoll_event ev;
// 	ev.data.fd = conv.fd;

// 	switch (mode) {
// 		case READ:
// 			ev.events = EPOLLIN;
// 			break;
// 		case WRITE:
// 			ev.events = EPOLLOUT;
// 			break;
// 		default:
// 		std::cerr << "Unknown mode in setClientInterest\n";
// 		return;
// 	}
// 	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, conv.fd, &ev) < 0) {
// 		std::cerr << "Failed to change interest mode on fd: " << conv.fd << std::endl;
// 	}
// }

void	Dispatcher::setEpollInterest(Conversation& conv, e_interest_mode mode) {

	switch (mode) {
		case READ:
		case WRITE:
			setClientInterest(conv, mode);
			break;
		case READ_EXEC_FD:
		case WRITE_EXEC_FD:
			setExecutorInterest(conv, mode);
			break;
	}
}

void	Dispatcher::removeClientFromEpoll(Conversation& conv) {

	int	clientFd = conv.fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, clientFd, NULL) < 0) {
		std::cerr << "Failed to remove fd " << conv.config.identity.host << " from epoll\n";
	}
}

void	Dispatcher::removeExecutorFdFromEpoll(Conversation& conv) {

	int executorFd = conv.fdToClose;

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, executorFd, NULL) < 0) {
		std::cerr << "Failed to remove executor fd " << conv.config.identity.host << " from epoll\n";
	}
	close(executorFd);
	_executorFds.erase(conv.fdToClose);
	conv.fdToClose = -1;
}

void	Dispatcher::dispatch(Conversation& conv) {

	std::cout << "Dispatcher called" << std::endl;
	if (conv.state == WRITE_CLIENT)
		conv.state = TO_SEND;
	else if (conv.state == READ_CLIENT)
		conv.state = TO_READ;
	else if (conv.state == READ_EXEC)
		conv.state = EXEC;
	else if (conv.state == WRITE_EXEC)
		conv.state = EXEC;

	while (true) {

		if (conv.state == TO_SEND) {
			_sender->execute(conv);
		}
		else if (conv.state == EXEC) {
			_executor->execute(conv);
		}
		else if (conv.state == TO_READ) {
			std::cout << "READER CALLED" << std::endl;
			_reader->execute(conv);
		}
		else if (conv.state == RESPONSE) {
			_responseBuilder->execute(conv);
		}
		else if (conv.state == IS_SENT) {
			_postSender->execute(conv);
		}
		else if (conv.state == PARSE || conv.state == PARSE_BODY || conv.state == EOF_CLIENT) {
			std::cout << "PARSER CALLED" << std::endl;
			_parser->execute(conv);
		}
		else if (conv.state == VALIDATE) {
			_validator->execute(conv);
		}
		else if (conv.state == READ_EXEC) {
			setEpollInterest(conv, READ_EXEC_FD);
			break ;
		}
		else if (conv.state == WRITE_EXEC) {
			setEpollInterest(conv, WRITE_EXEC_FD);
			break ;
		}
		else if (conv.state == READ_CLIENT) {
			setEpollInterest(conv, READ);
			break ;
		}
		else if (conv.state == WRITE_CLIENT) {
			setEpollInterest(conv, WRITE);
			break ;
		}
		else if (conv.state == FINISH) {
			removeClientFromEpoll(conv);
			break ;
		}
	}

	if (conv.fdToClose != -1) {
		removeExecutorFdFromEpoll(conv);
	}
}
