#include "Webserv.hpp"
#include "HttpRequest.hpp"
#include <string>
#include <sstream>
#include <csignal>

void signal_handler(int signal){
	Webserv::signalFlag = signal;
}



int main(int argc, char** argv){
    Logger::setLogFilename("");
    Logger::setLogLevel(SILENT);
    if(argc != 2)
		return 1;
	if (!argv)
		std::cerr << "ERROR" << std::endl;
	std::signal(SIGINT, signal_handler);
	Webserv webserv(argv[1]);
	return 0;
}

