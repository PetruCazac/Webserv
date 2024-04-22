#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <map>
#include <netdb.h>
#include <unistd.h>
#include <list>
#include <exception>
#include <fcntl.h>
#include "Logger.hpp"


typedef struct addrinfo addrinfo_t;
typedef struct sockaddr_storage sockaddr_storage_t;
typedef struct pollfd pollfd_t;

typedef struct SocketConfiguration {
    // SocketConfiguration(const ConfigurationFile& config_file);
    SocketConfiguration(std::string server_name, std::string listening_port, size_t max_data_size_incoming) : server_name(server_name), listening_port(listening_port), max_data_size_incoming(max_data_size_incoming){}
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
} SocketConfiguration;

typedef struct ServerConfiguration {
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
    LogLevel log_level;
    std::string log_filename;
    SocketConfiguration* server_socket_config;
} ServerConfiguration;
#endif