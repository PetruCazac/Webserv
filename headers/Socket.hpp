#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <vector>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Configuration.hpp"
#include <ctime>

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
		Socket(std::string& listening_port);
		Socket(int connection_fd);
		~Socket();

		void removeSocket(void);
		bool bindAndListen(void);
		int acceptIncoming(void);
		bool sendtoClient(const std::string* data);
		bool receive(int client_fd, void* buffer, size_t buffer_size, int& bytes_read);
		int getSockFd(void) const;
		void* get_in_addr(struct sockaddr *sa);
		bool setupAddrInfo(void);
		SocketType getSocketType(void) const;
		SocketStatus getSocketStatus(void) const;
		void setSocketStatus(SocketStatus status);
		HttpRequest* getHttpRequest(void) const;
		HttpResponse* getHttpResponse(void) const;
		void setNewHttpRequest(std::istream &inputRequest);
		void setNewHttpResponse(std::vector<ServerDirectives> &serverConfig);
		void setNewHttpResponse(size_t errorCode);
        time_t getLastAccessTime(void) const;

	private:
		std::string     _listen_port;
		int             _sockfd;
		addrinfo_t      *_addr_info;
		SocketType      _socket_type;
		SocketStatus    _socket_status;
		HttpRequest     *_http_request;
		HttpResponse	*_http_response;
        time_t          _last_access_time;
};

#endif

