#include "Server.hpp"
#include "HttpResponse.hpp"

int errorFlag = 0;

Server::Server(ServerDirectives& inputConfig, size_t client_max_body_size, size_t keepalive_timeout) : _client_max_body_size(client_max_body_size), _keepalive_timeout(keepalive_timeout) {
	_server_config.push_back(inputConfig);
	// LOG_INFO_NAME("Constructor called. Server starting...", input_config->_directives[translateDirectives(SERVERNAME)][0]);
	addListeningSocket();
}
Server::~Server() {
}

void Server::closeServer(void){
	LOG_DEBUG_NAME("Close Server Socket Map.", _server_config[0].server_name);
	std::map<int, Socket*>::iterator it;
	for (it = _socket_map.begin(); it != _socket_map.end(); ++it) {
		if (it->second != NULL) {
			delete it->second;
			it->second = NULL;
		}
	}
	_socket_map.clear();
	_poll_fd_vector.clear();
	LOG_DEBUG_NAME("Server Socket Map cleaned up.", _server_config[0].server_name);
}

void Server::addServerConfig(ServerDirectives& serverConfig){
	_server_config.push_back(serverConfig);
}


bool Server::addListeningSocket() {
	LOG_INFO_NAME("Server attempting to add listening socket.", _server_config[0].name);
	try{
		Socket *socket = new Socket(_server_config[0].listen_port);
		//Add socket to pollfd vector and map
		pollfd_t poll_fd;
		poll_fd.fd = socket->getSockFd();
		poll_fd.events = POLLIN;
		poll_fd.revents = 0;
		fcntl(poll_fd.fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
		_poll_fd_vector.push_back(poll_fd);
		_socket_map[poll_fd.fd] = socket;
	}
	catch(const SocketException& e){
		LOG_ERROR_NAME(std::string("Error creating listening socket: ") + e.what(), _server_config[0].server_name);
		return false;
	}
	LOG_INFO_NAME("Server successfully added listening socket: " + _server_config[0].listen_port, _server_config[0].server_name);
	return true;
}

const std::vector<pollfd_t>& Server::getPollFdVector() const {
	return _poll_fd_vector;
}

void Server::handleEvents(const std::vector<pollfd_t>& active_fds) {
	 for (size_t i = 0; i < active_fds.size(); ++i) {
			for (size_t j = 0; j < _poll_fd_vector.size(); ++j) {
				if (_poll_fd_vector[j].fd == active_fds[i].fd) {
					switch (_socket_map[_poll_fd_vector[j].fd]->getSocketType()){
						case SERVER:
							handleServerSocketEvents(active_fds[i]);
							break;
						case CLIENT:
							handleClientSocketEvents(active_fds[i]);
							_poll_fd_vector[j].revents = 0;
							break;
					}
			}
		}
	}
}

void Server::handleServerSocketEvents(const pollfd_t& poll_fd) {
	LOG_DEBUG_NAME("Handling server socket events.", _server_config[0].server_name);
	switch (_socket_map[poll_fd.fd]->getSocketStatus()) {
		case LISTEN_STATE:
			if (poll_fd.revents & POLLIN) {
				std::string client_ip;
				int connection_fd = _socket_map[poll_fd.fd]->acceptIncoming();
				if (connection_fd == -1) {
					LOG_ERROR_NAME("Failed to accept incoming connection.", _server_config[0].server_name);
					return;
				}
				pollfd_t new_poll_fd;
				new_poll_fd.fd = connection_fd;
				new_poll_fd.events = POLLIN;
				new_poll_fd.revents = 0;
				fcntl(new_poll_fd.fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
				_poll_fd_vector.push_back(new_poll_fd);
				_socket_map[new_poll_fd.fd] = new Socket(connection_fd);
				std::ostringstream oss;
				oss << "Accepted new incoming connection: " << connection_fd;
				LOG_INFO_NAME(oss.str(), _server_config[0].server_name);
			}
			break;
		case RECEIVE:
			break;
		case SEND_RESPONSE:
			break;
		case WAIT_FOR_RESPONSE:
			break;
	}
}

void Server::updatePollFdForWrite(int fd) {
	for (size_t i = 0; i < _poll_fd_vector.size(); ++i) {
		if (_poll_fd_vector[i].fd == fd) {
			_poll_fd_vector[i].events = POLLOUT;
			break;
		}
	}
}

void Server::updatePollFdForRead(int fd) {
	for (size_t i = 0; i < _poll_fd_vector.size(); ++i) {
		if (_poll_fd_vector[i].fd == fd) {
			_poll_fd_vector[i].events = POLLIN;
			break;
		}
	}
}

void Server::removeSocketFromMap(int fd) {
	if (_socket_map[fd] != NULL) {
		delete _socket_map[fd];
		close(fd);
		_socket_map.erase(fd);
		for (size_t i = 0; i < _poll_fd_vector.size(); ++i) {
			if (_poll_fd_vector[i].fd == fd) {
				_poll_fd_vector.erase(_poll_fd_vector.begin() + i);
				break;
			}
		}
	}
}

void Server::handleClientSocketEvents(const pollfd_t& poll_fd) {
	Socket* clientSocket = _socket_map[poll_fd.fd];
	time_t currentTime = time(NULL);
	double secondsElapsed = difftime(currentTime, clientSocket->getLastAccessTime());
	if (secondsElapsed > _keepalive_timeout) {
		LOG_DEBUG_NAME("Client connection timed out.", _server_config[0].server_name);
		removeSocketFromMap(poll_fd.fd);
		return;
	}
	switch (clientSocket->getSocketStatus()) {
		case RECEIVE:
			if (poll_fd.revents & POLLIN) {
				char buffer[_client_max_body_size];
				std::memset(buffer, 0, _client_max_body_size);
				int bytes_read = 0;
				if (!clientSocket->receive(poll_fd.fd, &buffer, _client_max_body_size, bytes_read)) {
					std::ostringstream oss;
					oss << "Failed to receive data from client: " << poll_fd.fd;
					LOG_ERROR_NAME(oss.str(), _server_config[0].server_name);
				}
				if (bytes_read == 0) {
					LOG_DEBUG_NAME("Connection closed.", _server_config[0].server_name);
					removeSocketFromMap(poll_fd.fd);
					return ;
				}else if (bytes_read == _client_max_body_size && buffer[bytes_read - 1] != '\0'){
					clientSocket->setNewHttpResponse(404);
					removeSocketFromMap(poll_fd.fd);
					// response.setBody("Request is too big");
					return;
				} else {
					_socket_map[poll_fd.fd]->addClientMessage(buffer);
					std::istringstream iss(_socket_map[poll_fd.fd]->getClientMessage());
					_socket_map[poll_fd.fd]->setNewHttpRequest(iss);
					if(!_socket_map[poll_fd.fd]->isCompleteMessage()){
						clientSocket->setSocketStatus(RECEIVE);
						updatePollFdForRead(poll_fd.fd);
						return;
					}
					LOG_INFO("Received Client Message");
					_socket_map[poll_fd.fd]->setNewHttpResponse(_server_config);
					LOG_INFO("Created Server Response");
					std::ostringstream oss;
					oss << "Received data: \033[33m\n" << buffer << "\033[0m\n";
					LOG_DEBUG_NAME(oss.str(), _server_config[0].server_name);
					// If you have to wait for CGI
					// clientSocket->setSocketStatus(WAIT_FOR_RESPONSE);
					// If you can send a response immediately
					clientSocket->setSocketStatus(SEND_RESPONSE);
					updatePollFdForWrite(poll_fd.fd);
				}
			}
			break;
		case WAIT_FOR_RESPONSE:
			break;
		case SEND_RESPONSE:
			if (poll_fd.revents & POLLOUT) {
				const std::string& responseStr = clientSocket-> getHttpResponse()->getResponse().str(); // Obtain the formatted response as a string
				LOG_DEBUG(responseStr);
				clientSocket->sendtoClient(&responseStr);
				LOG_INFO_NAME("Sent response to client.", _server_config[0].server_name);
				if (clientSocket->getHttpRequest()->isKeepAlive()) {
					updatePollFdForRead(poll_fd.fd);
					clientSocket->setSocketStatus(RECEIVE);
				} else {
					removeSocketFromMap(poll_fd.fd);
				}
			}
			break;
		case LISTEN_STATE:
			break;
	}
}

void Server::checkKeepAlive() {
	time_t currentTime = time(NULL);

	for (std::map<int, Socket*>::iterator it = _socket_map.begin(); it != _socket_map.end(); ) {
		if (it->second->getSocketType() == CLIENT) {
			double secondsElapsed = difftime(currentTime, it->second->getLastAccessTime());
			if (secondsElapsed > _keepalive_timeout) {
				LOG_DEBUG_NAME("Client connection timed out.", _server_config[0].server_name);
				int fd = it->first;
				++it;
				removeSocketFromMap(fd);
				continue;
			}
		}
		++it;
	}
}
