
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "UtilsHttp.hpp"
#include "Config.hpp"
#include <map>
#include <string>
#include <istream>

struct MethodsException {
	enum MethodErrors {
		CANNOT_OPEN_FILE,
		CODE_NOT_EXIST
	};

	MethodErrors err;

	explicit MethodsException(MethodErrors err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case CANNOT_OPEN_FILE: return "cannot open file by uri"; break;
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

	void setBody(std::fstream &file, std::string &path);

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

	// const std::string &getResponse();
	const std::stringstream &getResponse() const;
};

#endif // HTTPRESPONSE_HPP
