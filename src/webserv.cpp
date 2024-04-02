#include "webserv.hpp"

void	init(char **argc){
	if (!argc)
		std::cerr << "ERROR" << std::endl;
		// Check the config file or default path to the config folder
}

int main(int argc, char** argv){
	if(argc != 2)
		return 1;
	init(argv);
	
}