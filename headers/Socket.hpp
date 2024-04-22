#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Configuration.hpp"
#include <vector>

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

enum SocketType {
    SERVER,
    CLIENT
};

enum SocketStatus {
    LISTEN,
    RECEIVE,
    WAIT_FOR_RESPONSE
};
class Socket {
    public:
        Socket(SocketConfiguration *input_config);
        Socket(int connection_fd);
        ~Socket();

        void removeSocket();
        bool bindAndListen();
        int acceptIncoming();
        bool sendtoClient(const std::string* data, size_t len);
        bool receive(int client_fd, void* buffer, size_t buffer_size, int& bytes_read);
        int getSockFd() const;
        void* get_in_addr(struct sockaddr *sa);
        bool setupAddrInfo();
        SocketType getSocketType() const;

    private:
        SocketConfiguration *socket_config;
        int _sockfd;
        addrinfo_t *_addr_info;
        SocketType _socket_type;
        SocketStatus _socket_status;


};

#endif

