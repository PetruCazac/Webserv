
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "config/Config.hpp"
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
	const std::string &getStatusCode(const int code);
};

struct HttpResponseExceptions {
	enum ResponceErrors {
		ERR
	};

	ResponceErrors err;

	explicit HttpResponseExceptions(ResponceErrors err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case ERR: return "status code not found"; break;
			default: return "unknown error"; break;
		}
	}
};

class HttpResponse {
private:
	std::stringstream _response;

	HttpResponse();

public:
	HttpResponse(const int code);
	HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request);
	// void addHeader(const std::string &header);
	std::string setErrorBody(const int code);
	const std::stringstream &getResponse() const;
};

#endif // HTTPRESPONSE_HPP
