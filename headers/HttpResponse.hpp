
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>
#include <istream>

#include "HttpRequest.hpp"
#include "Config.hpp"

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
		std::istream _response;
		HttpRequest& _request;

		HttpResponse();

	public:
		HttpResponse(size_t status_code);
		HttpResponse(std::vector<ServerDirectives> &serverConfig, HttpRequest& request);
		void addHeader(const std::string &header);
		void setBody(const std::vector<uint8_t> &body);
		void runGetMethod(void);
		void runPutMethod(void);
		void runDeleteMethod(void);
		void runErrorMethod(void);
		void checkAllowedMethod(void);
		const std::istream &getResponse() const;

		void makeDefaultErrorPage(size_t statsCode);
};

#endif // HTTPRESPONSE_HPP
