#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <deque>
#include <fstream>
#include <sstream>


// #include "Webserv.hpp"
#include "Directives.hpp"
#include "Logger.hpp"

typedef struct s_server{
	std::string name;
	std::map<std::string, std::vector<std::string> >	_directives;
	std::map<std::string, std::vector<std::string> >	_location_directives;
	std::vector<std::string>	_location_methods;
}	ServerDirectives;

struct Block {
	std::string name;
	std::vector<std::string> methods;
	std::vector<std::string> parameters;
	std::vector<Block> children;
};

// Static Function
std::string translateDirectives(enum Parser directive);
Parser getParseLevel(const std::string *str);


class Config {
	private:
		std::vector<ServerDirectives>	_serversConfig;
		std::vector<std::string> tokens;
		size_t	tokenIndex;

	public:
		Config();
		Block	block;
		~Config();
		Config(const Config& c);
		Config& operator=(const Config& c);

		void tokenize(const char* configFile);
		void parse(void);
		Block parseDirective();
		void checkFilename(const char* configFile);
		void parseConfig(Block& block);
		
		// Helper functions
		bool isBlock(void);
		bool isValidDirective(std::string str);
		
		// Getter Funciton
		std::vector<ServerDirectives> getServerConfig(void);
		
		// Printing functions
		void printDirective(Block& block, int i);
		void printConfig();

		// Exception functions
		class ParsingExceptions : public std::exception{};
		class OpenException : public ParsingExceptions{
			public:
				const char* what() const throw(){
					return "Error opening config file: ";}
		};
		class EmptyConfFile : public ParsingExceptions{
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