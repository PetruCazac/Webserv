
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "UtilsHttp.hpp"
#include "Config.hpp"
#include <map>
#include <string>
#include <istream>
#include <fcntl.h>
#include <fcntl.h>

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
	std::stringstream _response;
	std::string	_body;
	std::string	_contentType;
	std::map<std::string, std::string> _headers;
	int			_cgi_pipe_fd;
	bool		_is_cgi_response;
	pid_t		_cgi_pid;
	std::string _cgi_output;


	HttpResponse();

	// GET Method
	void	runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	void	findLocationUri(const std::vector<LocationDirectives>& locations, const std::string& uri, LocationDirectives& location);
	void	composeLocalUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path);
	void	handleAutoindex(const char* path);
	void	handleCGI(const ServerDirectives& server, const HttpRequest& request);

	// DELETE Method
	void	runDeleteMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	void	composeDeleteUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path);

	// POST Method
	void	runPostMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	void	composePostUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path);
	void	handleMultipart(const HttpRequest& request, std::string& path);
	bool	storeFormData(const std::string& formData, std::string& path);
	std::string	urlDecode(const std::vector<uint8_t>& bodyString);
	bool	handlePackage(std::string& package, std::string& path);
	std::string	createFilename(std::string& path, std::map<std::string, std::string>& metadata);

	// Response Constituting Functions
	void	readFile(std::string &path);
	void	makeDefaultResponse(const int code);
	std::string getErrorBody(const int code);
	void	setHeader(const std::string &header, const std::string &value);
	void	setResponse();

	// Helper Functions
	bool	isMethodAllowed(const ServerDirectives& server, const std::string method);
	bool	isMethodAllowed(const LocationDirectives& location, const std::string method);
	bool	isDirectory(const char* path);
	bool	isFile(const char* path);
	bool	checkAutoindex(ServerDirectives& server);
	void	chooseServerConfig(const std::vector<ServerDirectives>& config, const HttpRequest &request, ServerDirectives& server);
	bool	isValidPath(const char* path);
	std::string findCgiType(std::string& request);
	bool isIndex(std::string &path, ServerDirectives& config);


public:
	HttpResponse(const int code);
	HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	bool	isCGI(const std::string &uri);
	void 	setCgiEnvironment(const HttpRequest& request, const std::string& scriptPath, std::vector<const char*>& env);
	void	setCgiResponse(const int cgi_pipe_fd, pid_t cgi_pid, const bool is_cgi_response);
	std::string getFilePath(const std::string& scriptPath, std::string root) const;
	int		getCgiPipeFd() const;
	pid_t	getCgiPid() const;
	void	appendCgiOutput(const std::string &data);
	void	finalizeCgiResponse();
	bool	isCgiResponse() const;
	std::string	parseArguments(const HttpRequest& request);

	const std::stringstream &getResponse() const;
};

#endif // HTTPRESPONSE_HPP
