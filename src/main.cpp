#include "Logger.hpp"
#include "Server.hpp"

// int main() {
//     Logger::setLogLevel(DEBUG);
//     Logger::setLogFilename("log.txt");
//     LOG_INFO("Hello, world!");
//     LOG_DEBUG("This is a debug message.");
//     LOG_ERROR("This is an error message.");
//     LOG_WARNING("This is a warning message.");
//     return 0;
// }

int main(){
    Logger::setLogLevel(DEBUG);
    Logger::setLogFilename("");
    ServerConfiguration server_config;
    SocketConfiguration socket_config("localhost", "3490", 1024);
    server_config.server_socket_config = &socket_config;
    Server server(&server_config);
}