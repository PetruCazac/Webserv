#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"


typedef struct s_server{
	std::string					_port;
	std::vector<std::string>	_location;
	std::string					_server_name;
	std::string					_root;
	std::string					_include;
	std::string					_timeout;
	std::string					_cgiTieout
}	server;

class Config {
private:
	typedef std::deque<std::string> stringDeque;
	typedef std::deque<std::string>::iterator iteratorDeque;
	std::vector<server>	_Servers;
	Config();

public:
	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	// Parser main functions
	void getConfig(stringDeque directives);
	void getServerConfig(std::string filebuff);


	// Helper Functions
	bool openBracket(const iteratorDeque& it);
	bool closedBracket(const iteratorDeque& it);
	void checkFilename(const char* configFile);
	void checkBrackets(const std::deque<std::string>& directives) const;

	// Exception functions
	class ParsingExceptions : public std::exception{};
	class OpenException: public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Error opening config file: ";}
	};
	class InvalidFilename : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Wrong Config File: ";}
	};
	class WrongBracket : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Brackets are not well set: ";}
	};
};

#endif