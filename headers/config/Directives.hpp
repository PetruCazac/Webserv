#ifndef DIRECTIVES_HPP
#define DIRECTIVES_HPP


enum Parser {
	AUTOINDEX,
	CLIENT_MAX_BODY_SIZE,
	ERROR_PAGE,
	FASTCGI_PARAM,
	INDEX,
	KEEP_ALIVE_TIMEOUT,
	LIMIT_EXCEPT,
	LISTEN,
	LOG_FILE,
	ROOT,
	SEND_TIMEOUT,
	SERVER_NAME,
	TOTAL
};

// const char* Directives[TOTAL] = {
// 	"autoindex",
// 	"client_max_body_size",
// 	"error_page",
// 	"fastcgi_param",
// 	"index",
// 	"keep_alive_timeout",
// 	"limit_except",
// 	"listen",
// 	"log_file",
// 	"root",
// 	"send_timeout",
// 	"server_name"
// };


#endif