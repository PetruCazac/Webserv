
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

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
	int _code;
	std::istream _response;

	HttpResponse();

public:
	explicit HttpResponse(const int code);
	void addHeader(const std::string &header);
	void setBody(const std::vector<uint8_t> &body);

	const std::istream &getResponse() const;
};

#endif // HTTPRESPONSE_HPP
