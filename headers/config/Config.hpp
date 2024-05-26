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

#include "Logger.hpp"
#include "DefaultValues.hpp"

typedef struct s_http{
	std::string name;
	int	keepalive_timeout;
	int	send_timeout;
	size_t	client_max_body_size;
}	HttpDirectives;

typedef struct s_location{
	std::string					module;
	std::string					autoindex;
	std::string					index;
	std::vector<std::string>	allow;
	std::string					root;
	std::string					post_dir;
}	LocationDirectives;

typedef struct s_server{
	std::string					name;
	std::string					server_name;
	std::string					autoindex;
	std::vector<std::string>	fastcgi_params;
	std::string					index;
	std::vector<std::string>	allow;
	std::string					listen_port;
	std::string					log_file; // Not used NEED TO ADAPT CONFIG.CPP
	std::string					root;
	std::string					post_dir;
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
		class ParsingException : public std::exception{};
		// class ParsingException : public std::runtime_error{
			// private:
				// std::string errorMessage;
			// public:
				// ParsingException(const char* error, ...);
				// const char* what() const throw();
		// };
		class OpenException : public ParsingException{
			public:
				const char* what() const throw(){
					return "Error opening config file: ";}
		};
		class EmptyConfFile : public ParsingException{
			public:
				const char* what() const throw(){
					return "Error: empty config file ";}
		};
		class InvalidFilename : public ParsingException{
			public:
				const char* what() const throw(){
					return "Wrong Config File: ";}
		};
		class WrongBracket : public ParsingException{
			public:
				const char* what() const throw(){
					return "Brackets are not well set: ";}
		};
		class WrongDirective : public ParsingException{
			public:
				const char* what() const throw(){
					return "Directive non valid: ";}
		};
		class MissingDirective : public ParsingException{
			public:
				const char* what() const throw(){
					return "Directive non existent ";}
		};
		class WrongMethods : public ParsingException{
			public:
				const char* what() const throw(){
					return "Non valid methods found: ";}
		};
		class MissingLastCharacter : public ParsingException{
			public:
				const char* what() const throw(){
					return "Invalid ending, missing: ";}
		};
		class WrongLocationDeclaration : public ParsingException{
			public:
				const char* what() const throw(){
					return "Location declared at the wrong place: ";}
		};
		class WrongServerDeclaration : public ParsingException{
			public:
				const char* what() const throw(){
					return "Server declared at the wrong place: ";}
		};
		class WrongDirectiveAttributes : public ParsingException{
			public:
				const char* what() const throw(){
					return "Wrong directive attributes ";}
		};
};

#endif