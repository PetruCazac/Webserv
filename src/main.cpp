#include "Logger.hpp"

int main() {
    Logger::setLogLevel(DEBUG);
    LOG_INFO("Hello, world!");
    LOG_DEBUG("This is a debug message.");
    return 0;
}