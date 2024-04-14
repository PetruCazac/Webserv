#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

class Server {
    public:
        Server(SocketConfiguration *input_config);
        ~Server();

        bool start();
        void stop();

    private:
        Socket *socket;
};

#endif