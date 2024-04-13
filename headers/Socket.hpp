#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Configuration.hpp"

class Socket {
    public:
        Socket();
        ~Socket();
        void connect();
        void send();
        void receive();
        void close();
};

#endif