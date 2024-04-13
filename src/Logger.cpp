#include "Logger.hpp"

void Logger::log(LogLevel level, const std::string &message, int line,
 const std::string &file) {
    std::cout << "[" << get_log_level(level) << "] " << message << " at line " 
    << line << " in file " << file << std::endl;
}

std::string Logger::get_log_level(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}