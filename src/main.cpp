
#include "Logger.hpp"
#include "Server.hpp"

int main(){
    Logger::setLogLevel(DEBUG);
    Logger::setLogFilename("");
    ServerConfiguration server_config;
    SocketConfiguration socket_config("localhost", "3490", 1024);
    server_config.server_socket_config = &socket_config;
    Server server(&server_config);
    server.run();
}
