#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Http.hpp"
#include "Config.hpp"

class Server {
    public:
        Server(ServerDirectives *input_config);
        ~Server();

        bool addListeningSocket();
        void socketHandler();
        void run();

    private:
        std::vector<pollfd_t> _poll_fd_vector;
        ServerConfiguration *_server_config;
        std::map<int, Socket*> _socket_map;
        std::map<int, Http*> _http_sessions;
        void convert_directives_to_config(ServerDirectives *input_config);
};

#endif