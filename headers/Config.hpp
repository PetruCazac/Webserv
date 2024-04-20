#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"

// enum {
// 	INDEX,
// 	LISTEN,
// 	LOCATION,
// 	HOSTNAME,
// 	SERVERNAME,
// 	CLIENTSIZE,
// 	PORT,
// 	ROOT,
// 	TOTAL
// };

// const char* Directives[TOTAL] = {
// 	"index",
// 	"listen",
// 	"location",
// 	"host_name",
// 	"server_name",
// 	"client_max_body_size",
// 	"port",
// 	"root"
// };

typedef struct s_server{
	std::vector<std::string>	_listen;
	std::vector<std::string>	_location;
	std::vector<std::string>	_server_name;
	std::vector<std::string>	_root;
	std::vector<std::string>	_client_max_body_size;
	std::vector<std::string>	_include;
	std::vector<std::string>	_timeout;
	std::vector<std::string>	_cgiTieout;
}	Server;

struct Block {
	std::string name;
	std::vector<std::string> methods;
	std::vector<std::string> parameters;
	std::vector<Block> children;
};

class Config {

private:
	Config();
	std::vector<Server>	_servers;
	std::vector<std::string> tokens;
	size_t	tokenIndex;

public:
	Block	block;
	Config(const char* configFile);
	~Config();
	Config(const Config& c);
	Config& operator=(const Config& c);

	void parse(void);
	Block parseDirective();
	void checkFilename(const char* configFile);
	void printDirective(Block& block, int i);
	bool isBlock(void);
	void parseConfig(Block& block);
	// void assignMethods(std::vector<std::string>& methods, Server& s);
	void assignDirective(std::vector<std::string>& parameters, Server& s);
	// void assignChildren(Block& children, Server& s);

	size_t fillDirective(std::vector<std::string>& s, std::vector<std::string>& parameters, size_t i);

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
	class WrongMethods : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Non valid methods found: ";}
	};
	class MissingLastCharacter : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Invalid ending, missing ;";}
	};
};

#endif