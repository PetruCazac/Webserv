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
	typedef std::deque<std::string> stringDeque;
	typedef std::deque<std::string>::iterator iteratorDeque;

public:
	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	// Parser main functions
	void getConfig(stringDeque& directives);
	void getServerConfig(std::string& filebuff);
	iteratorDeque& getBlocks(stringDeque& directives, iteratorDeque& it);

	// Helper Functions
	bool openBracket(const iteratorDeque& it);
	bool closedBracket(const iteratorDeque& it);
	
	// Exception functions
	class ParserProblem: public std::exception{
		public:
			const char* what() const throw(){
				return "Error at parsing";
			};
	};
};

#endif