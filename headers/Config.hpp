#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"

enum SERVER_TYPE
	{
		EVENTS,
		HTTP,
		MAIL,
		STREAM
	};


typedef struct s_config{
	std::string	_user;
	std::string	_worker_processes;
	std::string	_error_log;
	std::string	_pid;
}	config;

typedef struct s_server{
	
	std::string	_port;
	std::string	_location;
	std::string	_server_name;
	std::string	_root;
	std::string	_include;
} server;

class Config {
private:
	config				_engine_config;
	std::vector<server>	_Servers;

public:
	Config();
	~Config();
	Config& Config(const Config c);
	Config& operator=(const Config c);


};


#endif