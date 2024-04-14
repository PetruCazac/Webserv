#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"
#include <fstream>
#include <sstream>
#include <deque>

typedef struct s_server{
	std::string					_port;
	std::vector<std::string>	_location;
	std::string					_server_name;
	std::string					_root;
	std::string					_include;
}	server;

class Config {
private:
	std::vector<server>	_Servers;
	Config();

public:
	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	// Parser main functions
	void getConfig(std::string& confString);
	void getServerConfig(std::string& filebuff);
	std::& getBlocks(std::string& confString);
	
	// Parser helper funcitons

};

#endif