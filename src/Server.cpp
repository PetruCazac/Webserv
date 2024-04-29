#include "Server.hpp"

// Server::Server(ServerDirectives *input_config) {
//     LOG_INFO_NAME("Constructor called. Server starting...", input_config->_directives[translateDirectives(SERVERNAME)][0]);
//     convert_directives_to_config(input_config);
//     addListeningSocket();
// }


// void Server::convert_directives_to_config(ServerDirectives *input_config) {
//     std::string tmp = input_config->_directives[translateDirectives(SERVERNAME)][0];
//     _server_config.server_name = tmp;//input_config->_directives[translateDirectives(SERVERNAME)][0];
//     _server_config.listening_port = input_config->_directives[translateDirectives(LISTEN)][0];
//     // if(input_config->_directives.count(translateDirectives(MAX_DATA_SIZE_INC)) != 0){
//         // std::string debug_str = (input_config->_directives[translateDirectives(MAX_DATA_SIZE_INC)][0]);
//     //     std::stringstream str_tmp(debug_str);
//     // }
//     // size_t tmp;
//     // str_tmp >> tmp;
//     _server_config.max_data_size_incoming = 100000;//tmp;
//     Logger::setLogLevel(DEBUG);
//     Logger::setLogFilename("");
//     // _server_config->log_filename = input_config->_directives[translateDirectives(LOG_FILE)][0];
//     // _server_config->log_level = Logger::getLogLevel(input_config->_directives[translateDirectives(LOG_LEVEL)][0]);
//     _server_config.server_socket_config = new SocketConfiguration(_server_config.server_name, _server_config.listening_port, _server_config.max_data_size_incoming);

// // }

// void Server::run() {
//     LOG_INFO_NAME("Server running.", _server_config.server_name);
//     LOG_DEBUG_NAME("Server listening on port " + _server_config.listening_port, _server_config.server_name);
//     while (1){
//         socketHandler();
//     }
// }

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
    LOG_INFO_NAME("Server successfully added listening socket: " + _server_config.listening_port, _server_config.server_name);
    return true;
}

const std::vector<pollfd_t>& Server::getPollFdVector() const {
    return _poll_fd_vector;
}

void Server::handleEvents(const std::vector<pollfd_t>& active_fds) {
     for (size_t i = 0; i < active_fds.size(); ++i) {
            for (size_t j = 0; j < _poll_fd_vector.size(); ++j) {
                if (_poll_fd_vector[j].fd == active_fds[i].fd) {
                    switch (_socket_map[_poll_fd_vector[j].fd]->getSocketType()){
                        case SERVER:
                            handleServerSocketEvents(active_fds[i]);
                            break;
                        case CLIENT:
                            handleClientSocketEvents(active_fds[i]);
                            break;
                    }
            }
        }
     }
}

void Server::handleServerSocketEvents(const pollfd_t& poll_fd) {
    switch (_socket_map[poll_fd.fd]->getSocketStatus()) {
        case LISTEN_STATE:
            if (poll_fd.revents & POLLIN) {
                std::string client_ip;
                int connection_fd = _socket_map[poll_fd.fd]->acceptIncoming();
                if (connection_fd == -1) {
                    LOG_ERROR_NAME("Failed to accept incoming connection.", _server_config.server_name);
                    return;
                }
                pollfd_t new_poll_fd;
                new_poll_fd.fd = connection_fd;
                new_poll_fd.events = POLLIN;
                new_poll_fd.revents = 0;
                fcntl(new_poll_fd.fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC);
                _poll_fd_vector.push_back(new_poll_fd);
                _socket_map[new_poll_fd.fd] = new Socket(connection_fd);
                LOG_INFO_NAME("Accepted new incoming connection.", _server_config.server_name);
            }
            break;
        case RECEIVE:
            break;
        case SEND_RESPONSE:
            break;
        case WAIT_FOR_RESPONSE:
            break;
    }
}

void Server::updatePollFdForWrite(int fd) {
    for (size_t i = 0; i < _poll_fd_vector.size(); ++i) {
        if (_poll_fd_vector[i].fd == fd) {
            _poll_fd_vector[i].events = POLLOUT;
            break;
        }
    }
}

void Server::updatePollFdForRead(int fd) {
    for (size_t i = 0; i < _poll_fd_vector.size(); ++i) {
        if (_poll_fd_vector[i].fd == fd) {
            _poll_fd_vector[i].events = POLLIN;
            break;
        }
    }
}

void Server::handleClientSocketEvents(const pollfd_t& poll_fd) {
    switch (_socket_map[poll_fd.fd]->getSocketStatus()) {
        case RECEIVE:
            if (poll_fd.revents & POLLIN) {
                char buffer[100000] = {0};
                int bytes_read = 0;
                if (!_socket_map[poll_fd.fd]->receive(poll_fd.fd, &buffer, _server_config.max_data_size_incoming, bytes_read)) {
                    LOG_ERROR_NAME("Failed to receive data.", _server_config.server_name);
                }
                if (bytes_read == 0) {
                    LOG_DEBUG_NAME("Connection closed.", _server_config.server_name);
                    close(poll_fd.fd);
                    delete _socket_map[poll_fd.fd];
                    _socket_map.erase(poll_fd.fd);
                    _poll_fd_vector.erase(_poll_fd_vector.begin() + poll_fd.fd);
                    return ;
                } else {
                    std::istringstream iss(buffer);
                    _socket_map[poll_fd.fd]->setNewHttpRequest(iss);
                    std::ostringstream oss;
                    oss << "Received data: \033[33m\n" << buffer << "\033[0m\n";
                    LOG_DEBUG_NAME(oss.str(), _server_config.server_name);
                    // If you have to wait for CGI
                    // _socket_map[poll_fd.fd]->setSocketStatus(WAIT_FOR_RESPONSE);
                    // If you can send a response immediately
                    _socket_map[poll_fd.fd]->setSocketStatus(SEND_RESPONSE);
                    updatePollFdForWrite(poll_fd.fd);
                }
            }
            break;
        case WAIT_FOR_RESPONSE:
            break;
        case SEND_RESPONSE:
        {
            std::string body = "<html><body><h1>Hello, World!</h1></body></html>";
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\n"
                     << "Content-Type: text/html\r\n"
                     << "Content-Length: " << body.length() << "\r\n"
                     << "\r\n"  // Important: Blank line between headers and body
                     << body;

            const std::string& responseStr = response.str(); // Obtain the formatted response as a string
            _socket_map[poll_fd.fd]->sendtoClient(&responseStr, responseStr.length());
            _socket_map[poll_fd.fd]->setSocketStatus(RECEIVE); // Reset state if needed
            // TODO: Depending on keep alive or not, close the connection
            updatePollFdForRead(poll_fd.fd);
            LOG_INFO_NAME("Sent response to client.", _server_config.server_name);
        }
            break;
        case LISTEN_STATE:
            break;
    }
}