
#ifndef HTTPRESPONCE_HPP
#define HTTPRESPONCE_HPP

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

struct HttpResponceExceptions {
	enum ResponceErrors {
		ERR
	};

	ResponceErrors err;

	explicit HttpResponceExceptions(ResponceErrors err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case ERR: return "status code not found"; break;
			default: return "unknown error"; break;
		}
	}
};

class HttpResponce {
private:
	std::istream _responce;

public:
	explicit HttpResponce(const int code);
	void addHeader(const std::string &header);
	void setBody(const std::vector<uint8_t> &body);

	const std::istream &getResponce() const;
};

#endif // HTTPRESPONCE_HPP
