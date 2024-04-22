#include "Server.hpp"

Server::Server(ServerConfiguration *input_config) {
    LOG_INFO("Constructor called. Server starting...");
    _server_config = input_config;
    addListeningSocket();
}

void Server::run() {
    LOG_INFO("Server running.");
    while (1){
        socketHandler();
    }
}

Server::~Server() {

}

bool Server::addListeningSocket() {
    LOG_INFO("Server attempting to add listening socket.");
    try{
        Socket *socket = new Socket(_server_config->server_socket_config);
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
        LOG_ERROR(std::string("Error creating listening socket: ") + e.what());
        return false;
    }
    LOG_INFO("Server successfully added listening socket.");
    return true;
}

void Server::socketHandler() {
    int n_socket_events = poll(&_poll_fd_vector[0], _poll_fd_vector.size(), -1);
    // need to configure timeout
    if (n_socket_events == -1) {
        LOG_ERROR("Poll failed.");
        return;
    }
    for (size_t i = 0; i < _poll_fd_vector.size(); i++) {
        if (_poll_fd_vector[i].revents & POLLIN) {
            if (_socket_map[_poll_fd_vector[i].fd]->getSocketType() == SERVER) {
                std::string client_ip;
                int connection_fd = _socket_map[_poll_fd_vector[i].fd]->acceptIncoming();
                if (connection_fd == -1) {
                    LOG_ERROR("Failed to accept incoming connection.");
                    continue;
                }
                // Add new connection to pollfd vector and map
                pollfd_t poll_fd;
                poll_fd.fd = connection_fd;
                poll_fd.events = POLLIN;
                poll_fd.revents = 0;
                fcntl(poll_fd.fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
                _poll_fd_vector.push_back(poll_fd);
                _socket_map[poll_fd.fd] = new Socket(connection_fd);
                _http_sessions[poll_fd.fd] = new Http();
                LOG_INFO("Accepted new incoming connection.");
            } else {
                char buffer[10]={0};
                int bytes_read;
                if (!_socket_map[_poll_fd_vector[i].fd]->receive(_poll_fd_vector[i].fd, &buffer, _server_config->server_socket_config->max_data_size_incoming, bytes_read)) {
                    LOG_ERROR("Failed to receive data.");
                }
                if (bytes_read == 0) {
                    LOG_DEBUG("Connection closed.");
                    close(_poll_fd_vector[i].fd);
                    delete _socket_map[_poll_fd_vector[i].fd];
                    _socket_map.erase(_poll_fd_vector[i].fd);
                    _poll_fd_vector.erase(_poll_fd_vector.begin() + i);
                } else {
                    std::ostringstream oss;
                    oss << "Received data: \033[33m" << buffer << "\033[0m\n";
                    LOG_DEBUG(oss.str());
                }
            }
        }
    }
}