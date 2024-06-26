#include "Server.hpp"
#include "HttpResponse.hpp"

int errorFlag = 0;

Server::Server(ServerDirectives& inputConfig, size_t client_max_body_size, size_t keepalive_timeout) : _client_max_body_size(client_max_body_size), _keepalive_timeout(keepalive_timeout) {
	_server_config.push_back(inputConfig);
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
				if (_poll_fd_vector[j].fd == active_fds[i].fd || _socket_map[_poll_fd_vector[j].fd]->getSocketStatus() == WAIT_FOR_RESPONSE) {
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
            _poll_fd_vector[i].revents = 0;
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
    pid_t cgi_pid;
    int status;
    pid_t result_pid;
	Socket* clientSocket = _socket_map[poll_fd.fd];
	time_t currentTime = time(NULL);
	double secondsElapsed = difftime(currentTime, clientSocket->getLastAccessTime());
	if (secondsElapsed > _keepalive_timeout && clientSocket->getSocketType() == CLIENT) {
		LOG_DEBUG_NAME("Client connection timed out.", _server_config[0].server_name);
        if (clientSocket->getSocketStatus() == WAIT_FOR_RESPONSE) {
            cgi_pid = clientSocket->getHttpResponse()->getCgiPid();
            if (cgi_pid > 0) {
                std::stringstream oss;
                oss << "Killing CGI process due to timeout: " << cgi_pid;
                LOG_DEBUG_NAME(oss.str(), _server_config[0].server_name);
                kill(cgi_pid, SIGKILL);
                waitpid(cgi_pid, NULL, 0);
            }
            clientSocket->setAccessTime();
            clientSocket->setNewHttpResponse(408);
            clientSocket->setSocketStatus(SEND_RESPONSE);
            clientSocket->resetFlags();
            updatePollFdForWrite(poll_fd.fd);
            return;
        }
        removeSocketFromMap(poll_fd.fd);
		return;
	}
	switch (clientSocket->getSocketStatus()) {
		case RECEIVE:
			if (poll_fd.revents & POLLIN) {
				int bytes_read = 0;
				if (!clientSocket->receive(poll_fd.fd, bytes_read)) {
					std::ostringstream oss;
					oss << "Failed to receive data from client: " << poll_fd.fd;
					LOG_ERROR_NAME(oss.str(), _server_config[0].server_name);
				}
				if (bytes_read == 0 || bytes_read == -1) {
					LOG_DEBUG_NAME("Connection closed.", _server_config[0].server_name);
					removeSocketFromMap(poll_fd.fd);
					return ;
				}else if (clientSocket->getClientBodySize() >= static_cast<size_t>(_client_max_body_size)){
					clientSocket->setNewHttpResponse(413);
					clientSocket->setSocketStatus(SEND_RESPONSE);
					clientSocket->resetFlags();
					updatePollFdForWrite(poll_fd.fd);
					return;
				}else if(clientSocket->getSocketStatus() == RECEIVE){
					break;
				} else {
					std::istringstream iss(std::ios_base::binary);
					clientSocket->getClientMessage(iss);
					try {
						clientSocket->setNewHttpRequest(iss);
					} catch (const HttpRequestParserException &e) {
						clientSocket->setNewHttpResponse(400);
						clientSocket->setSocketStatus(SEND_RESPONSE);
						_socket_map[poll_fd.fd]->resetFlags();
						updatePollFdForWrite(poll_fd.fd);
						return ;
					}
					LOG_INFO("Received Client Message");
					clientSocket->setNewHttpResponse(_server_config);
					LOG_INFO("Created Server Response");
					std::ostringstream oss;
					LOG_DEBUG_NAME(oss.str(), _server_config[0].server_name);
                    if (clientSocket->getHttpResponse()->isCGI(clientSocket->getHttpRequest()->getUri())) {
                        clientSocket->_cgi_store = "";
                        clientSocket->_ready_send_cgi = false;
                        updatePollFdForWrite(poll_fd.fd);
                        clientSocket->setSocketStatus(WAIT_FOR_RESPONSE);
                    } else {
                        clientSocket->setSocketStatus(SEND_RESPONSE);
					    _socket_map[poll_fd.fd]->resetFlags();
                        updatePollFdForWrite(poll_fd.fd);
                    }
				}
			}
			break;
        case WAIT_FOR_RESPONSE:
            cgi_pid = clientSocket->getHttpResponse()->getCgiPid();
            result_pid = waitpid(cgi_pid, &status, WNOHANG);
            if (result_pid == -1 && !clientSocket->_ready_send_cgi) {
                LOG_ERROR("waitpid failed");
                clientSocket->getHttpResponse()->makeDefaultResponse(500);
                clientSocket->setSocketStatus(SEND_RESPONSE);
                updatePollFdForWrite(poll_fd.fd);
                return;
            } else if (result_pid > 0 && !clientSocket->_ready_send_cgi) {
                if (WIFEXITED(status)) {
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status != 0) {
                        LOG_ERROR("Child process exited with error status: ");
                        clientSocket->getHttpResponse()->makeDefaultResponse(500);
                        clientSocket->setSocketStatus(SEND_RESPONSE);
                        updatePollFdForWrite(poll_fd.fd);
                        return;
                    }
                } else if (WIFSIGNALED(status)) {
                    LOG_ERROR("Child process was terminated by signal: ");
                    clientSocket->getHttpResponse()->makeDefaultResponse(500);
                    clientSocket->setSocketStatus(SEND_RESPONSE);
                    updatePollFdForWrite(poll_fd.fd);
                    return;
                }
            }

            if (clientSocket->_ready_send_cgi || result_pid > 0) {
                clientSocket->_ready_send_cgi = true;
                LOG_DEBUG_NAME("Wait for response", _server_config[0].server_name);
                char buffer[1024];
                std::string cgiOutput;
                ssize_t bytesRead;
                int cgiPipeFd = clientSocket->getHttpResponse()->getCgiPipeFd();
                if ((bytesRead = read(cgiPipeFd, buffer, sizeof(buffer))) > 0) {
                    cgiOutput.append(buffer, bytesRead);
                    clientSocket->_cgi_store.append(buffer, bytesRead);
                    updatePollFdForWrite(poll_fd.fd);
                    clientSocket->setSocketStatus(WAIT_FOR_RESPONSE);
                    return ;
                }
                if (bytesRead == -1) {
                    std::ostringstream oss;
                    oss << "Failed to read CGI output from pipe: " << cgiPipeFd;
                    LOG_ERROR_NAME(oss.str(), _server_config[0].server_name);
                    removeSocketFromMap(poll_fd.fd);
                    return;
                }
                if (bytesRead == 0) {
                    close(cgiPipeFd);
                    clientSocket->getHttpResponse()->appendCgiOutput(clientSocket->_cgi_store);
                    clientSocket->getHttpResponse()->finalizeCgiResponse();
                    clientSocket->setSocketStatus(SEND_RESPONSE);
                    updatePollFdForWrite(poll_fd.fd);
                }
            }
            break;

		case SEND_RESPONSE:
			if (poll_fd.revents & POLLOUT) {
                clientSocket->setResponseStatus();
                if (!clientSocket->sendtoClient()) {
                    LOG_ERROR_NAME("Failed to send response to client.", _server_config[0].server_name);
                    removeSocketFromMap(poll_fd.fd);
                    return;
                }
                if (!clientSocket->isResponseSent()) {
                    clientSocket->setSocketStatus(SEND_RESPONSE);
                    updatePollFdForWrite(poll_fd.fd);
                    return ;
                }
				LOG_INFO_NAME("Sent response to client.", _server_config[0].server_name);
				if (clientSocket->hasHttpRequest() && clientSocket->getHttpRequest()->isKeepAlive()) {
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
                if (it->second->getSocketStatus() == WAIT_FOR_RESPONSE) {
                    pid_t cgi_pid = it->second->getHttpResponse()->getCgiPid();
                    if (cgi_pid > 0) {
                        std::stringstream oss;
                        oss << "Killing CGI process due to timeout: " << cgi_pid;
                        LOG_DEBUG_NAME(oss.str(), _server_config[0].server_name);
                        kill(cgi_pid, SIGKILL);
                        waitpid(cgi_pid, NULL, 0);
                    }
                    ++it;
                    continue;
                }
				++it;
				removeSocketFromMap(fd);
				continue;
			}
		}
		++it;
	}
}

