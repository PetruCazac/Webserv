#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <iostream>

#define LOG_SILENT(message) Logger::log(SILENT, message, __LINE__, __FILE__)
#define LOG_ERROR(message) Logger::log(ERROR, message, __LINE__, __FILE__)
#define LOG_WARNING(message) Logger::log(WARNING, message, __LINE__, __FILE__)
#define LOG_INFO(message) Logger::log(INFO, message, __LINE__, __FILE__)
#define LOG_DEBUG(message) Logger::log(DEBUG, message, __LINE__, __FILE__)

enum LogLevel {
    SILENT,
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
    public:
        static void log(LogLevel level, const std::string &message, int line,
         const std::string &file);
        static void setLogLevel(LogLevel level);
    private:
        static std::string translateLogLevel(LogLevel level);
        static LogLevel& getLogLevel();
};

#endif