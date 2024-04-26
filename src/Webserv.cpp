#include "Webserv.hpp"

Webserv::Webserv(char* str){
	try{
		_webserv_config.tokenize(str);
		_webserv_config.parse();
		init_servers();
		run_servers();
	} catch (Config::ParsingExceptions& e){
		std::cout << e.what() << str << std::endl;
		exit(1);
	}
}
Webserv::~Webserv(){}

void Webserv::init_servers(void){
	std::vector<ServerDirectives> serversConfig = _webserv_config.getServerConfig();
	if(!serversConfig.empty()){
		for (size_t i = serversConfig.size() - 1; ; --i){
			if(serversConfig[i].name == "http"){
				_http_directives = serversConfig[i]._directives;
			} else if(serversConfig[i].name == "server"){
				Server server(&serversConfig[i]);
				_servers.push_back(server);
			}
			if(i == 0)
				break;
		}
	}
}

void Webserv::run_servers(void){
	while(true){
        updatePollFds();
        if (poll(&_master_poll_fds[0], _master_poll_fds.size(), 100) > 0){
            for (size_t i = 0; i < _servers.size(); ++i){
                _servers[i].handleEvents(_master_poll_fds);
            }
        }
	}
}

void Webserv::updatePollFds(){
    _master_poll_fds.clear();
    for (size_t i = 0; i < _servers.size(); ++i){
        const std::vector<pollfd_t>& tmp = _servers[i].getPollFdVector();
        _master_poll_fds.insert(_master_poll_fds.end(), tmp.begin(), tmp.end());
    }
}