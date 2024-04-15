#include "Server.hpp"

Server::Server(ServerConfiguration &input_config) {
    _server_config = input_config;
    addListeningSocket();
}

bool Server::addListeningSocket() {
    Socket *socket = new Socket(_server_config.server_socket_config);
    pollfd_t poll_fd;
    poll_fd.fd = socket->getSockFd();
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;
    _poll_fd_vector.push_back(poll_fd);
    _socket_map[poll_fd] = socket;
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
                if (_socket_map[_poll_fd_vector[i]]->getSocketType() == SERVER) {
                    std::string client_ip;
                    int client_fd = _socket_map[_poll_fd_vector[i]]->acceptIncoming();
                    if (client_fd == -1) {
                        LOG_ERROR("Failed to accept incoming connection.");
                        continue;
                    }
                    pollfd_t poll_fd;
                    poll_fd.fd = client_fd;
                    poll_fd.events = POLLIN;
                    poll_fd.revents = 0;
                    _poll_fd_vector.push_back(poll_fd);
                    _socket_map[poll_fd] = new Socket(_server_config.server_socket_config);
                } else {
                    std::string buffer;
                    int bytes_read;
                    if (!_socket_map[_poll_fd_vector[i]]->receive(_poll_fd_vector[i].fd, &buffer, _server_config.server_socket_config->max_data_size_incoming, bytes_read)) {
                        LOG_ERROR("Failed to receive data.");
                        continue;
                    }
                    if (bytes_read == 0) {
                        LOG_DEBUG("Connection closed.");
                        close(_poll_fd_vector[i].fd);
                        _poll_fd_vector.erase(_poll_fd_vector.begin() + i);
                        delete _socket_map[_poll_fd_vector[i]];
                        _socket_map.erase(_poll_fd_vector[i]);
                    } else {
                        LOG_DEBUG("Received data: " + buffer);
                    }
                }
            }
        }
}