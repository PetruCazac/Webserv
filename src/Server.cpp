#include "Server.hpp"

Server::Server(ServerConfiguration *input_config) {
    server_config = input_config;
    addListeningSocket();
}

bool Server::addListeningSocket() {
    Socket *socket = new Socket(server_config->socket_config);
    pollfd_t poll_fd;
    poll_fd.fd = server_config->socket_config->sockfd;
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;
    poll_fds.push_back(poll_fd);
    socket_map[poll_fd] = socket;
    return true;
}

void Server::socketHandler() {
    while (true) {
        // need to configure timeout
        int poll_status = poll(&poll_fds[0], poll_fds.size(), -1);
        if (poll_status == -1) {
            LOG_ERROR("Poll failed.");
            return;
        }
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_config->socket_config->sockfd) {
                    std::string client_ip;
                    int client_fd = socket_map[server_config->socket_config->sockfd]->acceptIncoming(client_ip);
                    if (client_fd == -1) {
                        LOG_ERROR("Failed to accept incoming connection.");
                        continue;
                    }
                    pollfd_t poll_fd;
                    poll_fd.fd = client_fd;
                    poll_fd.events = POLLIN;
                    poll_fd.revents = 0;
                    poll_fds.push_back(poll_fd);
                    socket_map[poll_fd] = new Socket(server_config->socket_config);
                } else {
                    std::string buffer;
                    int bytes_read;
                    if (!socket_map[poll_fds[i]]->receive(poll_fds[i].fd, &buffer, server_config->socket_config->max_data_size_incoming, bytes_read)) {
                        LOG_ERROR("Failed to receive data.");
                        continue;
                    }
                    if (bytes_read == 0) {
                        LOG_DEBUG("Connection closed.");
                        close(poll_fds[i].fd);
                        poll_fds.erase(poll_fds.begin() + i);
                        delete socket_map[poll_fds[i]];
                        socket_map.erase(poll_fds[i]);
                    } else {
                        LOG_DEBUG("Received data: " + buffer);
                    }
                }
            }
        }
    }
}