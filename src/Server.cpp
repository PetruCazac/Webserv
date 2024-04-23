#include "Server.hpp"

Server::Server(ServerDirectives *input_config) {
    LOG_INFO_NAME("Constructor called. Server starting...", input_config->_directives[translateDirectives(SERVERNAME)][0]);
    convert_directives_to_config(input_config);
    addListeningSocket();
}


void Server::convert_directives_to_config(ServerDirectives *input_config) {
    std::string tmp = input_config->_directives[translateDirectives(SERVERNAME)][0];
    _server_config.server_name = tmp;//input_config->_directives[translateDirectives(SERVERNAME)][0];
    _server_config.listening_port = input_config->_directives[translateDirectives(LISTEN)][0];
    // if(input_config->_directives.count(translateDirectives(MAX_DATA_SIZE_INC)) != 0){
    //     std::string debug_str = (input_config->_directives[translateDirectives(MAX_DATA_SIZE_INC)][0]);
    //     std::stringstream str_tmp(debug_str);
    // }
    // size_t tmp;
    // str_tmp >> tmp;
    _server_config.max_data_size_incoming = 100000;//tmp;
    Logger::setLogLevel(DEBUG);
    Logger::setLogFilename("");
    // _server_config->log_filename = input_config->_directives[translateDirectives(LOG_FILE)][0];
    // _server_config->log_level = Logger::getLogLevel(input_config->_directives[translateDirectives(LOG_LEVEL)][0]);
    _server_config.server_socket_config = new SocketConfiguration(_server_config.server_name, _server_config.listening_port, _server_config.max_data_size_incoming);

}

void Server::run() {
    LOG_INFO_NAME("Server running.", _server_config.server_name);
    LOG_DEBUG_NAME("Server listening on port " + _server_config.listening_port, _server_config.server_name);
    while (1){
        socketHandler();
    }
}

Server::~Server() {}

bool Server::addListeningSocket() {
    LOG_INFO_NAME("Server attempting to add listening socket.", _server_config.server_name);
    try{
        Socket *socket = new Socket(_server_config.server_socket_config);
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
        LOG_ERROR_NAME(std::string("Error creating listening socket: ") + e.what(), _server_config.server_name);
        return false;
    }
    LOG_INFO_NAME("Server successfully added listening socket.", _server_config.server_name);
    return true;
}

void Server::socketHandler() {
    int n_socket_events = poll(&_poll_fd_vector[0], _poll_fd_vector.size(), -1);
    // need to configure timeout
    if (n_socket_events == -1) {
        LOG_ERROR_NAME("Poll failed.", _server_config.server_name);
        return;
    }
    for (size_t i = 0; i < _poll_fd_vector.size(); i++) {
        if (_poll_fd_vector[i].revents & POLLIN) {
            if (_socket_map[_poll_fd_vector[i].fd]->getSocketType() == SERVER) {
                std::string client_ip;
                int connection_fd = _socket_map[_poll_fd_vector[i].fd]->acceptIncoming();
                if (connection_fd == -1) {
                    LOG_ERROR_NAME("Failed to accept incoming connection.", _server_config.server_name);
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
                LOG_INFO_NAME("Accepted new incoming connection.", _server_config.server_name);
            } else {
                char buffer[100000]={0};
                int bytes_read;
                if (!_socket_map[_poll_fd_vector[i].fd]->receive(_poll_fd_vector[i].fd, &buffer, _server_config.server_socket_config->max_data_size_incoming, bytes_read)) {
                    LOG_ERROR_NAME("Failed to receive data.", _server_config.server_name);
                }
                if (bytes_read == 0) {
                    LOG_DEBUG_NAME("Connection closed.", _server_config.server_name);
                    close(_poll_fd_vector[i].fd);
                    delete _socket_map[_poll_fd_vector[i].fd];
                    _socket_map.erase(_poll_fd_vector[i].fd);
                    _poll_fd_vector.erase(_poll_fd_vector.begin() + i);
                } else {
                    std::ostringstream oss;
                    oss << "Received data: \033[33m" << buffer << "\033[0m\n";
                    LOG_DEBUG_NAME(oss.str(), _server_config.server_name);
                }
            }
        }
    }
}