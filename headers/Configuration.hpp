#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>

typedef struct ConfigurationFile {
    // Add Constructor that calls function to read from file
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
} ConfigurationFile;

typedef struct ServerConfiguration {
    ServerConfiguration(const ConfigurationFile& config_file);
    std::string server_name;
    std::string listening_port;
    size_t max_data_size_incoming;
} ServerConfiguration;

#endif