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
#include <exception>
#include <cstring>
#include <stdarg.h>

#include "Directives.hpp"
#include "Logger.hpp"
#include "DefaultValues.hpp"

typedef struct s_http{
	std::string name;
	int	keepalive_timeout;
	int	send_timeout;
}	HttpDirectives;

typedef struct s_location{
	std::string					module;
	std::string					autoindex;
	std::vector<std::string>	fastcgi_param;
	std::string					index;
	std::vector<std::string>	limit_except;
	std::string					root;
	std::map<int, std::string>	error_pages;
}	LocationDirectives;

typedef struct s_server{
	std::string					name;
	std::string					server_name;
	std::string					autoindex;
	double						client_max_body_size;
	std::string					index;
	int							listen;
	std::string					log_file;
	std::string					root;
	std::map<int, std::vector<std::string> >	error_page;
	std::vector<LocationDirectives>	locations;
}	ServerDirectives;

struct Block {
	std::string name;
	std::vector<std::string> methods;
	std::vector<std::string> parameters;
	std::vector<Block> children;
};

class Config {
	private:
		std::vector<std::string> tokens;
		size_t	tokenIndex;
		Block block;

	public:
		HttpDirectives	_httpConfig;
		std::vector<ServerDirectives>	_serversConfig;
		
		Config();
		~Config();
		Config(const Config& c);
		Config& operator=(const Config& c);

		void tokenize(const char* configFile);
		void parse(void);
		Block parseDirective();
		void checkFilename(const char* configFile);
		void parseConfig(Block& block);
		void parseHttp(HttpDirectives& server, Block& block);
		void parseServer(ServerDirectives& server, Block& block);
		void parseLocation(LocationDirectives& location, Block& block);

		// Helper functions
		bool isBlock(void);
		void getHttpStruct(HttpDirectives& http);
		void getServerStruct(ServerDirectives& server);
		void getLocationStruct(LocationDirectives& location);
		void checkServerDirectives(ServerDirectives& server);
		void checkLocationDirectives(LocationDirectives& location);


		// Printing functions
		void printDirective(Block& block, int i);
		void printConfig();
	
		// class ParsinExceptions : public std::runtime_exception()
		// Exception functions
		class ParsingException : public std::exception{
			private:
				std::string errorMessage;
			public:
				ParsingException(const char* error, ...);
				const char* what() const throw();
		};
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
					return "Invalid ending, missing: ";}
		};
		class WrongLocationDeclaration : public ParsingExceptions{
			public:
				const char* what() const throw(){
					return "Location declared at the wrong place: ";}
		};
		class WrongServerDeclaration : public ParsingExceptions{
			public:
				const char* what() const throw(){
					return "Server declared at the wrong place: ";}
		};
		class WrongDirectiveAttributes : public ParsingExceptions{
			public:
				const char* what() const throw(){
					return "Wrong directive attributes ";}
		};
};

#endif