#ifndef DEFAULT_VALUES_HPP
#define DEFAULT_VALUES_HPP

#include <string>
#include "Config.hpp"

enum ValuesEnum {
	AUTOINDEX,
	CLIENT_MAX_BODY_SIZE,
	ERROR_PAGE,
	FASTCGI_PARAM,
	INDEX,
	KEEP_ALIVE_TIMEOUT,
	ALLOW,
	LISTEN,
	LOG_FILE,
	ROOT,
	SEND_TIMEOUT,
	SERVER_NAME
};

class DefaultValues{
	private:
		static const std::string	_AUTOINDEX;
		static const size_t			_CLIENT_MAX_BODY_SIZE;  // MANDATORY
		static const std::string	_ERROR_PAGE;
		static const std::string	_FASTCGI_PARAM;
		static const std::string	_INDEX;
		static const int			_KEEP_ALIVE_TIMEOUT;  // MANDATORY
		static const std::string	_ALLOW;
		static const std::string	_LISTEN;  // MANDATORY
		static const std::string	_LOG_FILE;
		static const std::string	_ROOT;
		static const int			_SEND_TIMEOUT;  // For CGI Timeout.
		static const std::string	_SERVER_NAME;  // MANDATORY
	public:
		template<typename T>
		static T getDefaultValue(ValuesEnum directive);
};
#endif

// //--------------------- Http Config---------------------//
// Directives for http block :
// keepalive_timeout	-- time to keep a connection alive with a client
// send_timeout			-- timeout for CGI processes

// // ------------------- Server config -------------------//
// Directives for server block :
// autoindex				-- List all the directories present in the accessed directory.
// client_max_body_size		-- Maximum size of the body sent by the client.
// error_page				-- List all the default files that would be returned in case of an error, if not only the error code will be shown.
// index					-- If a file is not found it will go to the file defined by index and will serve it.
// listen					-- The number of the port which tghe server will operate on.
// log_file					-- The name of the file where the logs will be stored.
// root						-- The root directory where the files will be searched.
// server_name				-- The name of the server.

// // ---------------- Location config ----------------------//
// Directives for server block :
// autoindex			-- Command to show the directory listing.
// error_page			-- Defines the default error pages  for each error that occurs. If there is no file, a simple error will be returned.
// fastcgi_param		-- Limit the FAST_CGI methods that are allowed to be used in this location.
// index				-- If the page does not exist, the page defined by index will be returned. If there is no page, the error will be returned.
// allow				-- Limit the HTTP methods that are allowed to be used in this location.
// module				-- The directory from the root where the files will be searched.
// root					-- redefines the root path for the location it is being used.