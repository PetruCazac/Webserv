
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <iterator>
#include <stdlib.h>

enum HttpMethods {
	GET,
	HEAD,
	POST,
	PUT,
	TRACE,
	OPTIONS,
	DELETE
};

struct HttpRequestParserException {
	enum ErrorsHttpRequest {
		START_LINE_ERR,
		METHOD_ERR,
		URI_ERR,
		HTTP_VERSION_ERR,
		HEADER_ERR,
		CONTENT_LENGTH_ERR
	};

	ErrorsHttpRequest err;

	explicit HttpRequestParserException(ErrorsHttpRequest err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case START_LINE_ERR: return "start line is empty"; break;
			case METHOD_ERR: return "bad method"; break;
			case URI_ERR: return "bad URI"; break;
			case HTTP_VERSION_ERR: return "bad HTTP version"; break;
			case HEADER_ERR: return "bad header"; break;
			case CONTENT_LENGTH_ERR: return "content-length mismatch"; break;
			default: return "unknown error"; break;
		}
	}
};

class HttpRequest {
private:
	HttpMethods _method;
	std::string _uri;
	std::string _query;
	std::string _httpVersion;
	std::map<std::string, std::string> _headers;
	std::string _boundary;
	std::vector<uint8_t> _body;

	HttpRequest();

	void parseStartLine(const std::string &line);
	void parseHeaders(std::istream &inputRequest);
	void readBody(std::istream &inputRequest);
	void findQuery();
	void findBoundary();

	bool isValidHttpVersion() const;

public:
	explicit HttpRequest(std::istream &inputRequest);

	HttpMethods getMethod() const;
    const std::string getMethodStr() const;
    const std::string getQueryString() const;
    const std::string getContentLength() const;
    bool isKeepAlive() const;
	const std::string &getUri() const;
	const std::string &getQuery() const;
	const std::string &getHttpVersion() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBoundary() const;
	const std::vector<uint8_t> &getBody() const;
	bool isValidContentLength() const;
};

#endif // HTTPREQUEST_HPP
