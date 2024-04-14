#include "Socket.hpp"

Socket::Socket(SocketConfiguration *input_config){
    socket_config = input_config;
    if (!setupAddrInfo()) {
        LOG_ERROR("Failed to setup address info.");
    }
}

Socket::~Socket() {
    removeSocket();
    if (socket_config->addr_info != NULL) {
        freeaddrinfo(socket_config->addr_info);
        LOG_DEBUG("Address info freed.");
    }
    LOG_DEBUG("Socket destroyed.");
}

bool Socket::setupAddrInfo() {
    addrinfo_t hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, socket_config->listening_port.c_str(), &hints,
     &socket_config->addr_info);
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
    if (socket_config->sockfd != -1) {
        close(socket_config->sockfd);
        socket_config->sockfd = -1;
    }
    LOG_DEBUG("Socket closed.");
}

bool Socket::bindAndListen() {
    addrinfo_t *p;
    bool success = false;
    for (p = socket_config->addr_info; p != NULL; p = p->ai_next) {
        if (p == NULL) {
            LOG_ERROR("Address info not set up.");
            continue;
        }
        socket_config->sockfd = socket(p->ai_family,
        p->ai_socktype,
        p->ai_protocol);
        if (socket_config->sockfd == -1) {
            LOG_ERROR("Failed to create socket.");
            continue;
        }

        int yes = 1;
        if (setsockopt(socket_config->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            LOG_ERROR("Failed to set socket options.");
            removeSocket();
            continue;
        }

        if (bind(socket_config->sockfd, p->ai_addr,
        p->ai_addrlen) == -1) {
            LOG_ERROR("Failed to bind socket.");
            removeSocket();
            continue;
        }

        if (listen(socket_config->sockfd, 10) == -1) {
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
int Socket::acceptIncoming(std::string& client_ip) {
    sockaddr_storage_t client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_fd = accept(socket_config->sockfd, (struct sockaddr*)&client_addr, &addr_size);
    if (client_fd == -1) {
        LOG_ERROR("Failed to accept connection.");
        return -1;
    }
//Remove this before submission; Just for debugging purposes
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(client_addr.ss_family, &(((sockaddr_in*)&client_addr)->sin_addr), ip, sizeof(ip));
    client_ip = ip;
    LOG_DEBUG("Accepted connection from " + client_ip);
    return client_fd;
}

bool Socket::sendtoClient(const std::string* data, size_t len) {
    if (send(socket_config->sockfd,data->c_str(), len, 0) == -1) {
        LOG_ERROR("Failed to send data.");
        return false;
    }
    LOG_DEBUG("Successfully sent data.");
    return true;
}

// bool Socket::receive(std::string& buffer, size_t buffer_size, size_t& bytes_read) {
//     char buf[buffer_size];
//     bytes_read = recv(socket_config->sockfd, buf, buffer_size, 0);
//     if (bytes_read == -1) {
//         LOG_ERROR("Failed to receive data.");
//         return false;
//     }
//     buffer = std::string(buf, bytes_read);
//     LOG_DEBUG("Successfully received data.");
//     return true;
// }

bool Socket::receive(int client_fd, std::string* buffer, size_t buffer_size, int& bytes_read) {
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
    return bytes_read > 0;
}

void* Socket::get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}