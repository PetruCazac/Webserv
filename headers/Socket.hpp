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
        Socket(SocketConfiguration *input_config, SocketType socket_type);
        ~Socket();

        void removeSocket();
        bool bindAndListen();
        int acceptIncoming(std::string& client_ip);
        bool sendtoClient(const std::string* data, size_t len);
        bool receive(int client_fd, std::string* buffer, size_t buffer_size, int& bytes_read);
        // int getFd() const;
        void* get_in_addr(struct sockaddr *sa);
        bool setupAddrInfo();

    private:
        SocketConfiguration *socket_config;
        int _sockfd;
        addrinfo_t *_addr_info;
        SocketType _socket_type;


};

#endif