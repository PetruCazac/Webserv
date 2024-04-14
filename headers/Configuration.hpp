#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "Logger.hpp"

//Remove this Header before submission; Just for debugging purposes
#include <arpa/inet.h>


typedef struct addrinfo addrinfo_t;
typedef struct sockaddr_storage sockaddr_storage_t;
typedef struct ConfigurationFile {
    // Add Constructor that calls function to read from file
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
    LogLevel log_level;
    std::string log_filename;
} ConfigurationFile;

typedef struct SocketConfiguration {
    SocketConfiguration(const ConfigurationFile& config_file);
    SocketConfiguration(std::string server_name, std::string listening_port, size_t max_data_size_incoming) : server_name(server_name), listening_port(listening_port), max_data_size_incoming(max_data_size_incoming), sockfd(-1), addr_info(NULL) {}
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
    int sockfd;
    addrinfo_t* addr_info;
} SocketConfiguration;

#endif