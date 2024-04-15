#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server {
    public:
        Server(ServerConfiguration &input_config);
        ~Server();

        bool addListeningSocket();
        void socketHandler();

    private:
        std::vector<pollfd_t> _poll_fd_vector;
        ServerConfiguration &_server_config;
        std::map<pollfd_t, Socket*> _socket_map;
};

#endif