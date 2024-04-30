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

#include "Socket.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"

class Webserv{
	public:
		static int signalFlag;
		Webserv(char* str);
		~Webserv();

		void init_servers(void);
		void run_servers(void);

	private:
		Config _webserv_config;
		std::vector<Server> _servers;
		std::vector<pollfd> _master_poll_fds;
		void updatePollFds();
};

#endif
