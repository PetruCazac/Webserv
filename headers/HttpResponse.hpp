#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "Config.hpp"
#include <map>
#include <string>
#include <istream>

class StatusCodeMap {
private:
	std::map<int, std::string> statusCodes;

	StatusCodeMap();
	StatusCodeMap(const StatusCodeMap &other);
	StatusCodeMap &operator=(const StatusCodeMap &other);

public:
	static StatusCodeMap &getInstance();
	const std::string &getStatusCodeDescription(const int code);
};

struct HttpResponseExceptions {
	enum ResponceErrors {
		CODE_NOT_EXIST
	};

	ResponceErrors err;

	explicit HttpResponseExceptions(ResponceErrors err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case CODE_NOT_EXIST: return "status code not found"; break;
			default: return "unknown error"; break;
		}
	}
};

class HttpResponse {
private:
	std::stringstream	_response;
	HttpResponse();

	void makeDefaultErrorPage(const int code);
	ServerDirectives& findServer(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	FILE*	openFileByUri(const std::string &uri, std::vector<ServerDirectives> server);
	void	runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	void	findLocationUri(const std::vector<LocationDirectives>& locations, const std::string& uri, LocationDirectives& location);
	void	composeLocalUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path);
	void	handleAutoindex(const char* path);
	void	handleCGI(const ServerDirectives& server, const HttpRequest& request);

		void runPutMethod(void);
		void runDeleteMethod(void);
		void runErrorMethod(void);
		void checkAllowedMethod(void);

	// Helper Functions
	bool	isCGI(const std::string &uri);
	bool	isMethodAllowed(const ServerDirectives& server, const std::string method);
	bool	isMethodAllowed(const LocationDirectives& location, const std::string method);
	bool	isDirectory(const char* path);
	bool	isFile(const char* path);
	bool	checkAutoindex(ServerDirectives& server);
	std::string setErrorBody(const int code);

public:
	HttpResponse(const int code);
	HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	// void addHeader(const std::string &header);

	const std::stringstream &getResponse();
};

#endif // HTTPRESPONSE_HPP
