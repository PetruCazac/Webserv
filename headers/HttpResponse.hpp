
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
	std::string setErrorBody(const int code);
	FILE* openFileByUri(const std::string &uri, std::vector<ServerDirectives> server);
	bool isCGI(const std::string &uri);
	void handleCGI(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	void runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	ServerDirectives& findServer(const std::vector<ServerDirectives> &config, const HttpRequest &request);

		void runPutMethod(void);
		void runDeleteMethod(void);
		void runErrorMethod(void);
		void checkAllowedMethod(void);

public:
	HttpResponse(const int code);
	HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	// void addHeader(const std::string &header);

	const std::stringstream &getResponse() const;
};

#endif // HTTPRESPONSE_HPP
