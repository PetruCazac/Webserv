#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Configuration.hpp"
#include <vector>


enum SocketType {
    SERVER,
    CLIENT
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
        bool receive(int client_fd, std::string* buffer, size_t buffer_size, int& bytes_read);
        int getSockFd() const;
        void* get_in_addr(struct sockaddr *sa);
        bool setupAddrInfo();
        SocketType getSocketType() const;

    private:
        SocketConfiguration *socket_config;
        int _sockfd;
        addrinfo_t *_addr_info;
        SocketType _socket_type;


};

#endif