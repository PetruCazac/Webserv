#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <sys/time.h>
#include <iomanip>

#define LOG_SILENT(message) Logger::log(SILENT, message, __LINE__, __FILE__)
#define LOG_ERROR(message) Logger::log(ERROR, message, __LINE__, __FILE__)
#define LOG_WARNING(message) Logger::log(WARNING, message, __LINE__, __FILE__)
#define LOG_INFO(message) Logger::log(INFO, message, __LINE__, __FILE__)
#define LOG_DEBUG(message) Logger::log(DEBUG, message, __LINE__, __FILE__)
#define LOG_ERROR_NAME(message, server_name) Logger::log(ERROR, message, __LINE__, __FILE__, server_name)
#define LOG_WARNING_NAME(message, server_name) Logger::log(WARNING, message, __LINE__, __FILE__, server_name)
#define LOG_INFO_NAME(message, server_name) Logger::log(INFO, message, __LINE__, __FILE__, server_name)
#define LOG_DEBUG_NAME(message, server_name) Logger::log(DEBUG, message, __LINE__, __FILE__, server_name)

enum LogLevel {
    SILENT,
    WARNING,
    ERROR,
    INFO,
    DEBUG
};

class Logger {
    public:
        static void log(LogLevel level, const std::string &message, int line,
         const std::string &file, const std::string &server_name = "");
        static void setLogLevel(LogLevel level);
        static void setLogFilename(const std::string &file_name);
        static LogLevel getLogLevel(const std::string &str);
    private:
        static std::string translateLogLevel(LogLevel level);
        static LogLevel& getLogLevel();
        static std::string &getLogFilename();
        static std::ofstream &getLogStream();
};

#endif