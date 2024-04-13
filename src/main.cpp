#include "Logger.hpp"

int main() {
    Logger::setLogLevel(DEBUG);
    LOG_INFO("Hello, world!");
    LOG_DEBUG("This is a debug message.");
    LOG_ERROR("This is an error message.");
    LOG_WARNING("This is a warning message.");
    return 0;
}