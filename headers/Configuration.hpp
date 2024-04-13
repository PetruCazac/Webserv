#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include "Logger.hpp"
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
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
} SocketConfiguration;

#endif