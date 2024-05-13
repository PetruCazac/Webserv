#include "DefaultValues.hpp"

const std::string	DefaultValues::_AUTOINDEX = "on";
const size_t		DefaultValues::_CLIENT_MAX_BODY_SIZE = 1000;
const std::string	DefaultValues::_ERROR_PAGE = "404.html";
const std::string	DefaultValues::_FASTCGI_PARAM = "";
const std::string	DefaultValues::_LOG_FILE = "log_file.log";
const std::string	DefaultValues::_INDEX = "";
const int			DefaultValues::_KEEP_ALIVE_TIMEOUT = 10;
const std::string	DefaultValues::_ALLOW = "";
const std::string	DefaultValues::_LISTEN = "";  // MANDATORY
const std::string	DefaultValues::_ROOT = "";  // MANDATORY
const int		DefaultValues::_SEND_TIMEOUT = 10;
const std::string	DefaultValues::_SERVER_NAME = "";  // MANDATORY

template< >
std::string DefaultValues::getDefaultValue(enum ValuesEnum directive){
		switch(directive) {
		case ALLOW:
			return _ALLOW;
		case AUTOINDEX:
			return _AUTOINDEX;
		case ERROR_PAGE:
			return _ERROR_PAGE;
		case FASTCGI_PARAM:
			return _FASTCGI_PARAM;
		case INDEX:
			return _INDEX;
		case LOG_FILE:
			return _LOG_FILE;
		case LISTEN:
			return _LISTEN;
		case ROOT:
			return _ROOT;
		case SERVER_NAME:
			return _SERVER_NAME;
		default:
			return "Unknown directive";
	}
	return 0;
}
template< >
int DefaultValues::getDefaultValue(enum ValuesEnum directive){
		switch(directive) {
		case KEEP_ALIVE_TIMEOUT:
			return _KEEP_ALIVE_TIMEOUT;
		case SEND_TIMEOUT:
			return _SEND_TIMEOUT;
		default:
			return 0;
	}
	return 0;
}
template< >
size_t DefaultValues::getDefaultValue(enum ValuesEnum directive){
		switch(directive) {
		case CLIENT_MAX_BODY_SIZE:
			return _CLIENT_MAX_BODY_SIZE;
		default:
			return 0;
	}
	return 0;
}

// Parser getParseLevel(const std::string& str){
// 	if (str == "index") return INDEX;
// 	else if (str == "listen") return LISTEN;
// 	else if (str == "location") return LOCATION;
// 	else if (str == "host_name") return HOSTNAME;
// 	else if (str == "server_name") return SERVERNAME;
// 	else if (str == "client_max_body_size") return CLIENTSIZE;
// 	else if (str == "port") return PORT;
// 	else if (str == "root") return ROOT;
// 	else if (str == "try_files") return TRY_FILES;
// 	else if (str == "log_file") return LOG_FILE;
// 	else if (str == "max_data_size_incoming") return MAX_DATA_SIZE_INC;
// 	else if (str == "log_level") return LOG_LEVEL;
// 	else return TOTAL;
// }