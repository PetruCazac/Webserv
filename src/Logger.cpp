#include "Logger.hpp"

void Logger::log(LogLevel level, const std::string &message, int line,
 const std::string &file, const std::string &server_name) {
    if (level <= getLogLevel() && level != SILENT) {
        // std::ostringstream timestamp;
        // struct timeval tv;
        // gettimeofday(&tv, NULL);
        // struct tm *timeinfo = localtime(&tv.tv_sec);
        // timestamp << std::setfill('0')
        //         << std::setw(2) << timeinfo->tm_hour << ':'
        //         << std::setw(2) << timeinfo->tm_min << ':'
        //         << std::setw(2) << timeinfo->tm_sec << ':'
        //         << std::setw(3) << (tv.tv_usec / 1000); 
        std::ostream &out = (getLogFilename().empty()) ? std::cout : getLogStream();
        switch(level) {
            case INFO:
                out << "\033[32m";
                break;
            case DEBUG:
                out << "\033[34m";
                break;
            case WARNING:
                out << "\033[33m";
                break;
            case ERROR:
                out << "\033[31m";
                break;
            default:
                out << "\033[0m";
                break;
        }
        if (!server_name.empty()) {
            out << "[" << server_name << "] ";
        }
        // out << "[" << translateLogLevel(level) << "]\033[95m["
        // << timestamp.str() << "] \033[0m " << message << " | at line " 
        // << line << " in file " << file << std::endl;
        out << "[" << translateLogLevel(level) << "] \033[0m " << message << " | at line " 
        << line << " in file " << file << std::endl;
    }
}

LogLevel Logger::getLogLevel(const std::string &str){
    if (str == "SILENT")
        return SILENT;
    else if (str == "WARNING")
        return WARNING;
    else if (str == "ERROR")
        return ERROR;
    else if (str == "INFO")
        return INFO;
    return DEBUG;
}

std::string Logger::translateLogLevel(enum LogLevel level) {
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
    return "UNKNOWN";
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