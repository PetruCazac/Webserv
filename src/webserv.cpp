#include "Webserv.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
#define BACKLOG 128

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
	int		buffer = 1000;
	char	sb[buffer];
	size_t	bytes_read;

	bytes_read = read(socket_client, sb, buffer);
	std::cout << "Request:\n" << sb << std::endl;
	std::cout << "END REQUEST" << std::endl;
	std::string	input(sb);
	s_httpRequest httpRequest = parseHttpRequest(input);
	std::cout << "Method: [" << httpRequest.method << "]" << std::endl;
    std::cout << "URI: [" << httpRequest.uri << "]" << std::endl;
    std::cout << "HTTP Version: [" << httpRequest.httpVersion << "]" << std::endl;
    for (std::map<std::string, std::string>::iterator it = httpRequest.headers.begin(); it != httpRequest.headers.end(); it++) {
        std::cout << "Header: [" << it->first << ", " << it->second << "]" << std::endl;
    }
    std::cout << "Body: [" << httpRequest.body << "]" << std::endl;
    std::cout << "Status code: " << httpRequest.statusCode << std::endl;
	fflush(stdout);
	bzero(sb, buffer);
	std::cout << "here" << std::endl;
	// FILE *fd = fopen("test/index.html", "r");
	FILE *fd = NULL;
	// int i = 0;
	// if (i == 0){
	// 	fd = fopen("test/test1.html", "r");
	// 	i++;
	// }else
		fd = fopen("test/images.jpeg", "r");

	if (fd == NULL)
		std::cout << "file open error"  << std::endl;
	bzero(sb, buffer);
	bytes_read = fread(sb, sizeof(unsigned char), buffer, fd);
	std::string pading = "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\nContent-Length: [length in bytes of the image]\r\n\r\n";
	write(socket_client, pading.c_str(), pading.size());
	write(socket_client, sb, bytes_read);
	std::cout << "\n" << sb << "\n" << std::endl;
	close(socket_client);
	fclose(fd);
	std::cout << "Closed connection" << std::endl;
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

