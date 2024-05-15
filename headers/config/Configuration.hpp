#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <map>
#include <netdb.h>
#include <unistd.h>
#include <list>
#include <exception>
#include <fcntl.h>
#include "Logger.hpp"


typedef struct addrinfo addrinfo_t;
typedef struct sockaddr_storage sockaddr_storage_t;
typedef struct pollfd pollfd_t;

#endif