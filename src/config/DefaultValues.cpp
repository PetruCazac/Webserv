#include "DefaultValues.hpp"

const std::string	DefaultValues::AUTOINDEX = "on";
const double		DefaultValues::CLIENT_MAX_BODY_SIZE = 1000;  // MANDATORY
const std::string	DefaultValues::ERROR_PAGE = "404.html";
const std::string	DefaultValues::FASTCGI_PARAM = "GET";
const std::string	DefaultValues::LOG_FILE = "log_file.log";
const std::string	DefaultValues::INDEX = "index.html";
const int			DefaultValues::KEEP_ALIVE_TIMEOUT = 10; // MANDATORY
const std::string	DefaultValues::LIMIT_EXCEPT = "all";
const int			DefaultValues::LISTEN = 0;  // MANDATORY
const std::string	DefaultValues::ROOT = "/";
const int		DefaultValues::SEND_TIMEOUT = 10;  // For CGI Timeout.
const std::string	DefaultValues::SERVER_NAME = "server";  // MANDATORY

template< typename T>
T DefaultValues::getDefaultValue(enum ValuesEnum directive){
		switch(directive) {
		case ValuesEnum::AUTOINDEX:
			return AUTOINDEX;
		case ValuesEnum::CLIENT_MAX_BODY_SIZE:
			return CLIENT_MAX_BODY_SIZE;
		case ValuesEnum::ERROR_PAGE:
			return ERROR_PAGE;
		case ValuesEnum::FASTCGI_PARAM:
			return FASTCGI_PARAM;
		case ValuesEnum::INDEX:
			return INDEX;
		case ValuesEnum::KEEP_ALIVE_TIMEOUT:
			return KEEP_ALIVE_TIMEOUT;
		case ValuesEnum::LIMIT_EXCEPT:
			return LIMIT_EXCEPT;
		case ValuesEnum::LISTEN:
			return LISTEN;
		case ValuesEnum::LOG_FILE:
			return LOG_FILE;
		case ValuesEnum::ROOT:
			return ROOT;
		case ValuesEnum::SEND_TIMEOUT:
			return SEND_TIMEOUT;
		case ValuesEnum::SERVER_NAME:
			return SERVER_NAME;
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