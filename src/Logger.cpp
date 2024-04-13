#include "Logger.hpp"

void Logger::log(LogLevel level, const std::string &message, int line,
 const std::string &file) {
    if (level <= getLogLevel()) {
    std::cout << "[" << translateLogLevel(level) << "] " << message << " at line " 
    << line << " in file " << file << std::endl;
    }
}

std::string Logger::translateLogLevel(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

LogLevel& Logger::getLogLevel() {
    static LogLevel current_log_level = INFO;
    return current_log_level;
}

void Logger::setLogLevel(LogLevel level) {
    getLogLevel() = level;
}