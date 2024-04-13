#include "Logger.hpp"

void Logger::log(LogLevel level, const std::string &message, int line,
 const std::string &file) {
    if (level <= getLogLevel() && level != SILENT) {
        std::ostream &out = (getLogFilename().empty()) ? std::cout : getLogStream();
        out << "[" << translateLogLevel(level) << "] " << message << " at line " 
        << line << " in file " << file << std::endl;
    }
}

std::string Logger::translateLogLevel(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
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

std::ofstream &Logger::getLogStream() {
    static std::ofstream log_stream;
    return log_stream;
}

std::string &Logger::getLogFilename() {
    static std::string log_filename = "log.txt";
    return log_filename;
}

void Logger::setLogFilename(const std::string &file_name) {
    getLogFilename() = file_name;
    if (!file_name.empty()) {
        getLogStream().open(file_name.c_str(), std::ios::out);// | std::ios::app);
        if (!getLogStream().is_open()) {
            std::cerr << "Failed to open log file." << std::endl;
        }
    }
}