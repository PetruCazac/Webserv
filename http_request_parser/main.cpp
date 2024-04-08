#include "HttpRequest.hpp"
#include <iostream>
#include <sys/socket.h>

int main() {
	std::string request = "DELETE /test/hi-there.txt HTTP/1.3\r\nAccept: text/*\r\nContent-Length: 56\r\nHost: www.joes-hardware.com\r\n\r\nPlease go to our partner site, www.gentle-grooming.com\r\n";
	// std::string request = "GET /test/hi-there.txt HTTP/2\r\nAccept: text/*\r\n\r\nthis is body\r\n";
	s_httpRequest httpRequest = parseHttpRequest(request);

	std::cout << "[" << httpRequest.method << "]" << std::endl; // delete
	std::cout << "[" << httpRequest.uri << "]" << std::endl; // delete
	std::cout << "[" << httpRequest.httpVersion << "]" << std::endl; // delete
	for (std::map<std::string, std::string>::iterator it = httpRequest.headers.begin(); it != httpRequest.headers.end(); it++) { // delete
		std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
	}
	std::cout << "[" << httpRequest.body << "]" << std::endl; // delete
	std::cout << "status code " << httpRequest.statusCode << std::endl; // delete
	return 0;
}
