#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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

// #include "Server.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"

class Webserv{
	public:
		Webserv(char* str);
		~Webserv();

		void init_servers(std::vector<ServerDirectives>	_serversConfig);

	private:
		Config _webserv_config;
		std::vector<Server> _servers;
};

#endif