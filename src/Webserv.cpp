#include "Webserv.hpp"

Webserv::Webserv(char* str){
	try{
		signalFlag = 0;
		_webserv_config.tokenize(str);
		_webserv_config.parse();
		init_servers();
		run_servers();
	} catch (Config::ParsingException& e){
		std::cout << e.what() << str << std::endl;
		exit(1);
	}
}
Webserv::~Webserv(){}

void Webserv::init_servers() {
	std::vector<ServerDirectives> serversConfig = _webserv_config._serversConfig;
	if (!serversConfig.empty()) {
		for (size_t i = 0; i < serversConfig.size(); ){
			Server server(serversConfig[i], _webserv_config._httpConfig.client_max_body_size);
			std::string listen_port = serversConfig[i].listen_port;
			for (size_t j = i + 1; j < serversConfig.size(); ) {
				if (listen_port == serversConfig[j].listen_port) {
					server.addServerConfig(serversConfig[j]);
					serversConfig.erase(serversConfig.begin() + j);
				} else
					j++;
			}
			_servers.push_back(server);
			i++;
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
