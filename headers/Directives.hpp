#ifndef DIRECTIVES_HPP
#define DIRECTIVES_HPP


enum Parser {
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
	"root"
	"try_files",
	"log_file",
	"max_data_size_incoming",
	"log_level",
};


#endif