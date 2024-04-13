#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <iostream>

#define LOG_INFO(message) Logger::log(INFO, message, __LINE__, __FILE__)
#define LOG_DEBUG(message) Logger::log(DEBUG, message, __LINE__, __FILE__)

enum LogLevel {
    INFO,
    DEBUG
};

class Logger {
    public:
        static void log(LogLevel level, const std::string &message, int line,
         const std::string &file);
    private:
        static std::string get_log_level(LogLevel level);
};

#endif