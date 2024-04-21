
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <map>

typedef enum HttpMethods {
	NONE,
	GET,
	HEAD,
	POST,
	PUT,
	TRACE,
	OPTIONS,
	DELETE
}	e_HttpMethods;

struct Methods {
	std::string name;
	e_HttpMethods method;
};

struct HttpRequestParserException {
	enum ErrorsHttpRequest {
		NO_ERR,
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
			case START_LINE_ERR: return ": start line is empty"; break;
			case METHOD_ERR: return ": bad method"; break;
			case URI_ERR: return ": bad URI"; break;
			case HTTP_VERSION_ERR: return ": bad HTTP version"; break;
			case HEADER_ERR: return ": bad header"; break;
			case CONTENT_LENGTH_ERR: return ": content-length mismatch"; break;
			default: return "unknown error"; break;
		}
	}
};

class HttpRequest {
private:
	e_HttpMethods _method;
	std::string _uri;
	std::string _httpVersion;
	std::map<std::string, std::string> _headers;
	std::string _body;
	int _statusCode;

	void parseStartLine(std::string &line);
	void parseHeaders(std::istringstream &inputRequest);
	void parseBody(std::istringstream &inputRequest);

	e_HttpMethods methodToEnum(std::string &method) const;
	bool isHttpVersion() const;
	bool validContentLength() const;

public:
	HttpRequest(std::istringstream &inputRequest);

	e_HttpMethods getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	int getStatusCode() const;
};

#endif // HTTPREQUEST_HPP
