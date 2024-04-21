
// #include "Logger.hpp"
#include "new_parser.hpp"
#include <sstream>

int main() {
    // Logger::setLogLevel(DEBUG);
    // LOG_INFO("Hello, world!");
    // LOG_DEBUG("This is a debug message.");
    // LOG_ERROR("This is an error message.");
    // LOG_WARNING("This is a warning message.");

	// PARSER TESTING:
	// std::string input = "DELETE /test/hi-there.txt HTTP/1.1\r\nAccept: text/*\r\nContent-Length: 56\r\nHost: www.joes-hardware.com\r\n\r\nPlease go to our partner site, www.gentle-grooming.com\r\n";
	std::string input = "DELETE /test/hi-there.txt HTTP/1.1\r\nAccept: text/*\r\n\r\nthis is body\n{\n\"username\": \"john_doe\",\n\"email\": \"john.doe@example.com\"\n}\r\n";
	std::istringstream request(input);
	try {
		HttpRequest httpRequest(request);
		std::cout << "[" << httpRequest.getMethod() << "]" << std::endl;
		std::cout << "[" << httpRequest.getUri() << "]" << std::endl;
		std::cout << "[" << httpRequest.getHttpVersion() << "]" << std::endl;
		std::map<std::string, std::string> headers = httpRequest.getHeaders();
		if (headers.size() == 0)
			std::cout << "[no headers]" << std::endl;
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
			std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
		}
		if (httpRequest.getBody().size() == 0)
			std::cout << "[no body]" << std::endl;
		else
			std::cout << "[" << httpRequest.getBody() << "]" << std::endl;
		std::cout << "status code " << httpRequest.getStatusCode() << std::endl;
	} catch (const HttpRequestParserException &e) {
		std::cerr << "Error: bad HTTP request" << e.what() << std::endl;
	}

    return 0;
}
