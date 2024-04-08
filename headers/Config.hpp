#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"



typedef struct s_config{
	std::string	_user;
	std::string	_worker_processes;
	std::string	_error_log;
	std::string	_pid;
}	config;

class Config {
private:
	config				_engine_config;
	std::vector<Server>	_Servers;

public:

};


#endif
