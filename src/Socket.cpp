#include "Socket.hpp"

Socket::Socket(std::string& listen_port) : _listen_port(listen_port), _sockfd(-1), _http_request(NULL){
	_socket_type = SERVER;
	setSocketStatus(LISTEN_STATE);
	LOG_DEBUG("Constructor for listening Socket called.");
	if (!setupAddrInfo()) {
		LOG_ERROR("Failed to setup address info.");
		throw SocketException("Failed to setup address info.");
	}
	if (!bindAndListen()) {
		LOG_ERROR("Failed to bind and listen on socket.");
		throw SocketException("Failed to bind and listen on socket.");
	}
}

// Socket::Socket(int connection_fd) : socket_config(NULL), _sockfd(connection_fd), _addr_info(NULL), _http_request(NULL) {
Socket::Socket(int connection_fd) : _sockfd(connection_fd), _addr_info(NULL), _http_request(NULL) {
	_socket_type = CLIENT;
	setSocketStatus(RECEIVE);
}

Socket::~Socket() {
	removeSocket();
	if (_addr_info != NULL) {
		freeaddrinfo(_addr_info);
		LOG_DEBUG("Address info freed.");
	}
	LOG_DEBUG("Socket destroyed.");
}

bool Socket::setupAddrInfo() {
	LOG_DEBUG("Setting up address info.");
	addrinfo_t hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, _listen_port.c_str(), &hints,
	 &_addr_info);
	if (status != 0) {
		std::ostringstream oss;
		oss << "getaddrinfo error: " << gai_strerror(status);
		LOG_ERROR(oss.str());
		return false;
	}
	LOG_DEBUG("Successfully setup address info.");

	return true;
}

void Socket::removeSocket() {
	if (_sockfd != -1) {
		close(_sockfd);
		_sockfd = -1;
	}
	LOG_DEBUG("Socket closed.");
}

bool Socket::bindAndListen() {
	LOG_DEBUG("Attempting to bind and listen on socket.");
	addrinfo_t *p;
	bool success = false;
	for (p = _addr_info; p != NULL; p = p->ai_next) {
		if (p == NULL) {
			LOG_ERROR("Address info not set up.");
			continue;
		}
		_sockfd = socket(p->ai_family,
		p->ai_socktype,
		p->ai_protocol);
		if (_sockfd == -1) {
			LOG_ERROR("Failed to create socket.");
			continue;
		}

		int yes = 1;
		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			LOG_ERROR("Failed to set socket options.");
			removeSocket();
			continue;
		}

		if (bind(_sockfd, p->ai_addr,
		p->ai_addrlen) == -1) {
			LOG_ERROR("Failed to bind socket.");
			removeSocket();
			continue;
		}

		if (listen(_sockfd, 10) == -1) {
			LOG_ERROR("Failed to listen on socket.");
			removeSocket();
			continue;
		}
		success = true;
		break;
	}
	if (!success)
		return false;
	LOG_DEBUG("Successfully bound and listening on socket.");
	return true;
}

// probably add sockaddr_storage_t as a parameter
int Socket::acceptIncoming() {
	sockaddr_storage_t client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int client_fd = accept(_sockfd, (struct sockaddr*)&client_addr, &addr_size);
	if (client_fd == -1) {
		LOG_ERROR("Failed to accept connection.");
		return -1;
	}
	return client_fd;
}

bool Socket::sendtoClient(const std::string* data) {
	size_t len_sent = 0;
	int bytes_sent = 0;
	size_t len = data->length();
	while (len_sent < len) {
		bytes_sent = send(_sockfd, data->c_str() + len_sent, len - len_sent, 0);
		if (bytes_sent < 0) {
			LOG_ERROR("Failed to send data.");
			return false;
		}
		len_sent += bytes_sent;
	}
	LOG_DEBUG("Successfully sent data.");
	return true;
}


bool Socket::receive(int client_fd, void* buffer, size_t buffer_size, int& bytes_read) {
	bytes_read = recv(client_fd, buffer, buffer_size, 0);
	if (bytes_read == -1) {
		LOG_ERROR("Failed to receive data.");
		return false;
	}
	if (bytes_read == 0) {
		LOG_DEBUG("Connection closed by client.");
		return false;
	}
	LOG_DEBUG("Successfully received data.");
	setSocketStatus(WAIT_FOR_RESPONSE);
	return bytes_read > 0;
}

void* Socket::get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Socket::getSockFd() const {
	return _sockfd;
}

SocketType Socket::getSocketType() const {
	return _socket_type;
}

SocketStatus Socket::getSocketStatus() const {
	return _socket_status;
}

void Socket::setSocketStatus(SocketStatus status) {
	_socket_status = status;
}

HttpRequest* Socket::getHttpRequest() const {
	return _http_request;
}

void Socket::setNewHttpRequest(std::istream &inputRequest) {
	if (this->getHttpRequest() != NULL) {
		delete _http_request;
		_http_request = NULL;
	}
	_http_request = new HttpRequest(inputRequest);
}

void Socket::setNewHttpResponse(std::vector<ServerDirectives> &serverConfig){
	if (this->getHttpRequest() == NULL) {
		LOG_ERROR("The request is not available for a response.");
		return ;
	}
	_http_response = new HttpResponse(serverConfig, *_http_request);
}

void Socket::setNewHttpResponse(size_t errorCode){
	if (this->getHttpRequest() == NULL) {
		LOG_ERROR("The request is not available for a response.");
		return ;
	}
	_http_response = new HttpResponse(errorCode);
}