#include "Server.hpp"

Server::Server(ServerConfiguration *input_config) {
    server_config = input_config;
    start();
}

bool Server::start() {
    Socket *socket = new Socket(server_config->socket_config);
    pollfd_t poll_fd;
    poll_fd.fd = server_config->socket_config->sockfd;
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;
    poll_fds.push_back(poll_fd);
    socket_map[poll_fd] = socket;
    return true;
}