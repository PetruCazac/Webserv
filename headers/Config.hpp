#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"

enum C_TYPES{
	HTTP,
	UNDEF
};


const char INDEX[] = "index";
const char LISTEN[] = "listen";
const char LOCATION[] = "location";
const char HOSTNAME[] = "host_name";
const char SERVERNAME[] = "server_name";
const char PORT[] = "port";
const char ROOT[] = "root";


typedef struct s_server{
	C_TYPES						_type;
	std::string					_port;
	std::vector<std::string>	_location;
	std::string					_server_name;
	std::string					_root;
	std::string					_include;
	std::string					_timeout;
	std::string					_cgiTieout;
}	server;

class Config {
private:
	typedef std::deque<std::vector<std::string> > strDeque;
	typedef std::deque<std::vector<std::string> >::iterator itDeque;
	typedef std::deque<std::vector<std::string> >::const_iterator citDeque;
	Config();


public:

	std::vector<server>	_servers;

	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	// Parser main functions
	void getConfig(strDeque directives);
	itDeque getServerConfig(itDeque begin, itDeque end);
	itDeque parseBlock(itDeque& it, itDeque& it_end, strDeque& directives);
	itDeque parseLocation(itDeque it, itDeque end);
	void parseDirective(const char* dir, std::vector<std::string> str);

	// Helper Functions
	void checkFilename(const char* configFile);
	void checkBrackets(const strDeque& directives) const;
	bool lineComment(const std::string line) const;
	itDeque endBlock(itDeque it, itDeque end);
	bool contextBlock(std::vector<std::string>& line);
	bool serverBlock(std::vector<std::string>& line);

	// Exception functions
	class ParsingExceptions : public std::exception{};
	class OpenException: public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Error opening config file: ";}
	};
	class EmptyConfFile: public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Error: empty config file ";}
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
	class WrongDirective : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Directive non valid: ";}
	};
	class MissingLasCharacter : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Invalid ending, missing ;";}
	};
};

#endif