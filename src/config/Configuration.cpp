#include "Configuration.hpp"

// SocketConfiguration::SocketConfiguration(const ConfigurationFile& config_file) {
//     server_name = config_file.server_name;
//     listening_port = config_file.listening_port;
//     max_data_size_incoming = config_file.max_data_size_incoming;
// }

SocketConfiguration::    SocketConfiguration(const ServerConfiguration& server_config) : server_name(server_config.server_name), listening_port(server_config.listening_port), max_data_size_incoming(server_config.max_data_size_incoming){}
