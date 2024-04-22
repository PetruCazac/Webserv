#include "Webserv.hpp"

Webserv::Webserv(char* str){
	try{
		_webserv_config.tokenize(str);
		_webserv_config.parse();
		init_servers(_webserv_config.getServerConfig());
	} catch (Config::ParsingExceptions& e){
		std::cout << e.what() << str << std::endl;
		exit(1);
	}
}
Webserv::~Webserv(){}

void Webserv::init_servers(std::vector<ServerDirectives> _serversConfig){
	if(!_serversConfig.empty()){
		for (size_t i = _serversConfig.size() - 1; ; --i){
			if(_serversConfig[i].name == "http"){
				
			} else if(_serversConfig[i].name == "server"){
			
			}
			if(i == 0)
				break;
		}
	}
}