#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"
#include <fstream>
#include <sstream>


typedef struct s_server{
	std::string	_port;
	std::string	_location;
	std::string	_server_name;
	std::string	_root;
	std::string	_include;
}	server;

class Config {
private:
	std::vector<server>	_Servers;

public:
	Config();
	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	// Parser main functions
	void getConfig(std::string& confString);
	void getServerConfig(const std::string& filebuff);
	
	// Parser helper funcitons

};

#endif