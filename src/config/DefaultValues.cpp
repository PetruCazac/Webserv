#include "DefaultValues.hpp"

const std::string	DefaultValues::AUTOINDEX = "on";
const double		DefaultValues::CLIENT_MAX_BODY_SIZE = 1000;  // MANDATORY
const std::string	DefaultValues::ERROR_PAGE = "404.html";
const std::string	DefaultValues::FASTCGI_PARAM = "GET";
const std::string	DefaultValues::FILE_LOG = "log_file.log";
const std::string	DefaultValues::INDEX = "index.html";
const double		DefaultValues::KEEP_ALIVE_TIMEOUT = 10; // MANDATORY
const std::string	DefaultValues::LIMIT_EXCEPT = "all";
const int			DefaultValues::LISTEN = 80;  // MANDATORY
const std::string	DefaultValues::LOG_FILE = "log.log";
const std::string	DefaultValues::MODULE = "/";  //MANDATORY
const std::string	DefaultValues::ROOT = "/";
const double		DefaultValues::SEND_TIMEOUT = 10;  // For CGI Timeout.
const std::string	DefaultValues::SERVER_NAME = "server";  // MANDATORY
