#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server {
    public:
        Server(ServerConfiguration *input_config);
        ~Server();

        bool start();
        void stop();

    private:
        std::list<pollfd_t> poll_fds;
        ServerConfiguration *server_config;
        std::map<pollfd_t, Socket*> socket_map;
};

#endif