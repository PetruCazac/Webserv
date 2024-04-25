
#include "../headers/Webserv.hpp"
#include "../headers/HttpRequest.hpp"
#include "../headers/HttpResponce.hpp"
#include "../headers/getMethod.hpp"
#include <string>
#include <sstream>

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
#define BACKLOG 128

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

void	init(char **argc){
	if (!argc)
		std::cerr << "ERROR" << std::endl;
		// Check the config file or default path to the config folder
}

int initializeServer(){
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	SA_IN s_address;

	std::cout << server_socket << std::endl;
	if(server_socket != -1){
		s_address.sin_port = htons(9575);
		s_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		s_address.sin_family = AF_INET;
	} else {
		std::cout << "ERROR SOCKET FAULT" << std::endl;
		exit(1);
	}
	if(bind(server_socket, (SA*)&s_address, sizeof(s_address)) < 0){
		std::cout << "ERROR BINDING FAULT" << std::endl;
		exit(1);
	}
	if(listen(server_socket, BACKLOG) < 0){
		std::cout << "ERROR LISTENING FAULT" << std::endl;
		exit(1);
	}
	return server_socket;
}

int acceptClient(int socket_server){
	int addr_size = sizeof(SA_IN);
	SA_IN client_addr;
	int client_socket = accept(socket_server, (SA*)&client_addr, (socklen_t*)&addr_size);
	if(client_socket == -1){
		std::cout << "ERROR ACCEPTING CLIENT SOCKET" << std::endl;
		exit(1);
	}
	return client_socket;
}

void doStuff(int socket_client){
	static const int buffer = 1000;
	char	sb[buffer];
	size_t	bytes_read;

	bytes_read = read(socket_client, sb, buffer);
	std::string	input(sb);
	std::istringstream request(input);
	try {
		HttpRequest httpRequest(request);
		std::cout << "--- REQUEST START ---" << std::endl;
		printHttpRequest(httpRequest);
		std::cout << "--- REQUEST END ---" << std::endl;
		FILE *file = openFileByUri(httpRequest.getUri());
		bzero(sb, buffer);
		bytes_read = fread(sb, sizeof(unsigned char), buffer, file);
	} catch (const HttpRequestParserException &e) {
		std::cerr << "Error: bad HTTP request: " << e.what() << std::endl;
	} catch (const MethodsException &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	} catch (const HttpResponceExceptions &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	write(socket_client, sb, bytes_read);
	close(socket_client);
	std::cout << "--- CONNECTION CLOSED ---" << std::endl << std::endl;
	return;
}

int main(int argc, char** argv){
	if(argc != 2)
		return 1;
	init(argv);
	int socket_server = initializeServer();
	while (true){
		int socket_client = acceptClient(socket_server);
		doStuff(socket_client);
	}
	close(socket_server);
	return 0;
}
