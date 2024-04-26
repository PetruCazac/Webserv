#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Config.hpp"

class Server {
    public:
        Server(ServerDirectives *input_config);
        ~Server();

        bool addListeningSocket();
        const std::vector<pollfd_t>& getPollFdVector() const;
        void handleEvents(const std::vector<pollfd_t>& active_fds);
        void handleServerSocketEvents(const pollfd_t& poll_fd);
        void handleClientSocketEvents(const pollfd_t& poll_fd);
        void updatePollFdForWrite(int fd);

    private:
        std::vector<pollfd_t> _poll_fd_vector;
        ServerConfiguration _server_config;
        std::map<int, Socket*> _socket_map;
        void convert_directives_to_config(ServerDirectives *input_config);
};

#endif