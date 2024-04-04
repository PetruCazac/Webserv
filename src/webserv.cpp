#include "webserv.hpp"

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
	size_t	bites_read;

	bites_read = read(socket_client, sb, buffer);
	std::cout << sb << std::endl;
	std::cout << bites_read << std::endl;
	return;
}

int main(int argc, char** argv){
	if(argc != 2)
		return 1;
	init(argv);
	int socket_server = initializeServer();
	std::cout << "here" << std::endl;
	while (true){
		int socket_client = acceptClient(socket_server);
		doStuff(socket_client);
	}
	return 0;
}

