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


enum {
	INDEX,
	LISTEN,
	LOCATION,
	HOSTNAME,
	SERVERNAME,
	CLIENTSIZE,
	PORT,
	ROOT,
	TRY_FILES,
	LOG_FILE,
	MAX_DATA_SIZE_INC,
	LOG_LEVEL,
	TOTAL
};

const char* Directives[TOTAL] = {
	"index",
	"listen",
	"location",
	"host_name",
	"server_name",
	"client_max_body_size",
	"port",
	"try_files",
	"log_file",
	"max_data_size_incoming",
	"log_level",
	"root"
};

// class Http{
// private:


// public:


// };

#endif