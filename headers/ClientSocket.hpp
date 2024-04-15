#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "Socket.hpp"

class ConnectionSocket : public Socket {
    public:
        ConnectionSocket(SocketConfiguration *input_config);
        ~ConnectionSocket();
        bool connectToServer();
        bool sendtoServer(const std::string* data, size_t len);
        bool receiveFromServer(std::string* buffer, size_t buffer_size, int& bytes_read);
};

#endif