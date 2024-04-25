#ifndef DEFAULT_VALUES_HPP
#define DEFAULT_VALUES_HPP

#include <string>

class DefaultValues{
	public:
		static const std::string	AUTOINDEX;
		static const double			CLIENT_MAX_BODY_SIZE;  // MANDATORY
		static const std::string	ERROR_PAGE;
		static const std::string	FASTCGI_PARAM;
		static const std::string	FILE_LOG;
		static const std::string	INDEX;
		static const double			KEEP_ALIVE_TIMEOUT; // MANDATORY
		static const std::string	LIMIT_EXCEPT;
		static const int			LISTEN;  // MANDATORY
		static const std::string	LOG_FILE;
		static const std::string	MODULE;  //MANDATORY
		static const std::string	ROOT;
		static const double			SEND_TIMEOUT;  // For CGI Timeout.
		static const std::string	SERVER_NAME;  // MANDATORY
}
#endif

// //--------------------- Http Config---------------------//
// Directives for http block :
// keepalive_timeout	-- time to keep a connection alive with a client
// send_timeout 		-- timeout for CGI processes

// // ------------------- Server config -------------------//
// Directives for server block :
// autoindex 				-- List all the directories present in the accessed directory.
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
// limit_except			-- Limit the HTTP methods that are allowed to be used in this location.
// module				-- The directory from the root where the files will be searched.
// root					-- redefines the root path for the location it is being used.
