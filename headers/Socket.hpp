#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Configuration.hpp"
#include <vector>
#include "HttpRequest.hpp"

class SocketException : public std::exception {
private:
	std::string message;
public:
	SocketException(const std::string& msg) : message(msg) {}
	virtual ~SocketException() throw() {} 
	virtual const char* what() const throw() {
		return message.c_str();
	}
};

typedef struct SocketConfiguration {
	// SocketConfiguration(std::string server_name, std::string listening_port, size_t max_data_size_incoming) : server_name(server_name), listening_port(listening_port), max_data_size_incoming(max_data_size_incoming){}
	// SocketConfiguration(const ServerConfiguration& server_config);// : server_name(server_config.server_name), listening_port(server_config.listening_port), max_data_size_incoming(server_config.max_data_size_incoming){}
	// std::string server_name;
	std::string listening_port;
	size_t max_data_size_incoming;
} SocketConfiguration;


enum SocketType {
	SERVER,
	CLIENT
};

enum SocketStatus {
	LISTEN_STATE,
	RECEIVE,
	WAIT_FOR_RESPONSE,
	SEND_RESPONSE
};

class Socket {
	public:
		// Socket(std::string& listening_port, size_t& _client_max_body_size);
		Socket(std::string& listening_port);
		Socket(int connection_fd);
		~Socket();

		void removeSocket(void);
		bool bindAndListen(void);
		int acceptIncoming(void);
		bool sendtoClient(const std::string* data, size_t len);
		bool receive(int client_fd, void* buffer, size_t buffer_size, int& bytes_read);
		int getSockFd(void) const;
		void* get_in_addr(struct sockaddr *sa);
		bool setupAddrInfo(void);
		SocketType getSocketType(void) const;
		SocketStatus getSocketStatus(void) const;
		void setSocketStatus(SocketStatus status);
		HttpRequest* getHttpRequest(void) const;
		void setNewHttpRequest(std::istream &inputRequest);

	private:
		// SocketConfiguration *socket_config;
		std::string     _listen_port;
		// size_t          _client_max_body_size;
		int             _sockfd;
		addrinfo_t      *_addr_info;
		SocketType      _socket_type;
		SocketStatus    _socket_status;
		HttpRequest     *_http_request;
};

#endif

