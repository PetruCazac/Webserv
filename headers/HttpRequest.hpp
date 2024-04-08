
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <map>

typedef enum ErrorsHttpRequest {
	NO_ERR,
	METHOD_ERR,
	URI_ERR,
	HTTPVERSION_ERR,
	HEADER_ERR,
	// BODY_ERR,
	CONTENT_LENGTH_ERR
}	e_errors;

typedef struct HttpRequest {
	std::string method;
	std::string uri;
	std::string httpVersion;
	std::map<std::string, std::string> headers;
	std::string body;
	int statusCode;
}	s_httpRequest;

s_httpRequest parseHttpRequest(std::string &httpRequest);
e_errors parseStartLine(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfStart);
e_errors parseHeaders(s_httpRequest &httpRequest, std::string &inputRequest, size_t &crlfHeader, size_t size);
e_errors parseBody(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfBody, size_t size);
bool ifMethod(std::string &method);
bool ifHttpVersion(std::string &httpVersion);
bool ifValidBody(s_httpRequest &httpRequest);
void error(int &statusCode, e_errors err);

#endif // HTTPREQUEST_HPP

/*
TODO
- check headers if needed
- redo errors, should be exceptions
*/
