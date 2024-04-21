
// #ifndef HTTPREQUEST_HPP
// #define HTTPREQUEST_HPP
#pragma once

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
		// BODY_ERR,
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
			case HEADER_ERR: return ": bad header(s)"; break;
			// case BODY_ERR: return ": body"; break;
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
	// void parseHeaders(std::istringstream &inputRequest);
	// void parseBody(std::istringstream &inputRequest);

	e_HttpMethods methodToEnum(std::string &method) const;
	bool isHttpVersion() const;

public:
	HttpRequest(std::istringstream &inputRequest);

	e_HttpMethods getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	int getStatusCode() const;
};

// s_httpRequest parseHttpRequest(std::string &httpRequest);
// e_errors parseStartLine(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfStart);
// e_errors parseHeaders(s_httpRequest &httpRequest, std::string &inputRequest, size_t &crlfHeader, size_t size);
// e_errors parseBody(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfBody, size_t size);
// bool ifMethod(std::string &method);
// bool ifHttpVersion(std::string &httpVersion);
// bool ifValidBody(s_httpRequest &httpRequest);
// void error(int &statusCode, e_errors err);

// #endif // HTTPREQUEST_HPP
