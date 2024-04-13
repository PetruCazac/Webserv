#include "Configuration.hpp"

SocketConfiguration::SocketConfiguration(const ConfigurationFile& config_file) {
    server_name = config_file.server_name;
    listening_port = config_file.listening_port;
    max_data_size_incoming = config_file.max_data_size_incoming;
}