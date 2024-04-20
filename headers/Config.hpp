#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Webserv.hpp"

typedef struct s_server{
	std::string name;
	std::map<std::string, std::vector<std::string> >	_directives;
	std::map<std::string, std::vector<std::string> >	_location_directives;
	std::vector<std::string>	_location_methods;
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
	void parseConfig(Block& block);
	
	// Helper functions
	bool isBlock(void);
	bool isValidDirective(std::string str);
	// Printing functions
	void printDirective(Block& block, int i);
	void printConfig();

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
	class MissingDirective : public ParsingExceptions{
		public:
			const char* what() const throw(){
				return "Directive non existent ";}
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