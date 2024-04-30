#include "Webserv.hpp"
#include "HttpRequest.hpp"
#include <string>
#include <sstream>
#include <csignal>

// typedef struct sockaddr_in SA_IN;
// typedef struct sockaddr SA;
// #define BACKLOG 128


void signal_handler(int signal){
	Webserv::signalFlag = signal;
}

// To be put in a class ////////////////////////////////////////

struct Methods { // delete
	std::string name;
	HttpMethods method;
};

void printMethod(HttpMethods method) { // delete
	Methods methods[] = {
		{"GET", GET},
		{"HEAD", HEAD},
		{"POST", POST},
		{"PUT", PUT},
		{"TRACE", TRACE},
		{"OPTIONS", OPTIONS},
		{"DELETE", DELETE}
	};
	const int size = sizeof(methods) / sizeof(methods[0]);
	for (int i = 0; i != size; i++) {
		if (method == methods[i].method)
			std::cout << "[" << methods[i].name << "]" << std::endl;
	}
}

void printHttpRequest(HttpRequest &httpRequest) { // delete
	printMethod(httpRequest.getMethod());
	std::cout << "[" << httpRequest.getUri() << "]" << std::endl;
	std::cout << "[" << httpRequest.getHttpVersion() << "]" << std::endl;
	std::map<std::string, std::string> headers = httpRequest.getHeaders();
	if (headers.size() == 0)
		std::cout << "[no headers]" << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
		std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
	}
	std::vector<uint8_t> body = httpRequest.getBody();
	if (body.size() == 0)
		std::cout << "[no body]" << std::endl;
	else {
		std::cout << "[";
		for (std::vector<uint8_t>::iterator it = body.begin(); it != body.end(); it++) {
			std::cout << *it;
		}
		std::cout << "]";
		std::cout << std::endl;
	}
}


int main(int argc, char** argv){
	if(argc != 2)
		return 1;
	if (!argv)
		std::cerr << "ERROR" << std::endl;
	std::signal(SIGINT, signal_handler);
	Webserv webserv(argv[1]);
	return 0;
}

