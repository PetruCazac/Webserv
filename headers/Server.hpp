#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server {
    public:
        Server(ServerConfiguration *input_config);
        ~Server();

        bool addListeningSocket();
        void socketHandler();

    private:
        std::vector<pollfd_t> poll_fds;
        ServerConfiguration *server_config;
        std::map<pollfd_t, Socket*> socket_map;
};

#endif