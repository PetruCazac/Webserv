
#include "new_parser.hpp"
// #include "HttpRequest.hpp"
#include <iostream>			// std::cout
#include <string>			// std::string
#include <sstream>			// std::istringstream
// #include <algorithm>
// #include <cstdlib>
// #include <map>

HttpRequest::HttpRequest(std::istringstream &inputRequest) {
	std::string line;
	std::getline(inputRequest, line);
	if (line.empty() || line[0] == '\r')
		throw HttpRequestParserException(HttpRequestParserException::START_LINE_ERR);
	parseStartLine(line);
	// parseHeaders(inputRequest);
	// parseBody(inputRequest);
}

void HttpRequest::parseStartLine(std::string &line) {
	_method = NONE;
	std::string method;
	std::istringstream startLine(line);
	startLine >> method;
	startLine >> _uri;
	startLine >> _httpVersion;
	_method = methodToEnum(method);
	if (_method == NONE)
		throw HttpRequestParserException(HttpRequestParserException::METHOD_ERR);
	if (_uri.size() == 0 || _uri[0] != '/')
		throw HttpRequestParserException(HttpRequestParserException::URI_ERR);
	if (!isHttpVersion())
		throw HttpRequestParserException(HttpRequestParserException::HTTP_VERSION_ERR);
}

// void parseHeaders(std::istringstream &inputRequest) {
// 	(void)inputRequest;
// }

// void parseBody(std::istringstream &inputRequest) {
// 	(void)inputRequest;
// }

e_HttpMethods HttpRequest::methodToEnum(std::string &method) const {
	Methods validMethods[] = {
		{"GET", GET},
		{"HEAD", HEAD},
		{"POST", POST},
		{"PUT", PUT},
		{"TRACE", TRACE},
		{"OPTIONS", OPTIONS},
		{"DELETE", DELETE}
	};
	const int size = sizeof(validMethods) / sizeof(validMethods[0]);
	for (int i = 0; i != size; i++) {
		if (method == validMethods[i].name)
			return validMethods[i].method;
	}
	return NONE;
}

bool HttpRequest::isHttpVersion() const {
	std::string validHttpVersions[5] = {"HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"};
	for (int i = 0; i != 5; i++) {
		if (_httpVersion == validHttpVersions[i])
			return true;
	}
	return false;
}

e_HttpMethods HttpRequest::getMethod() const {
	return _method;
}

std::string HttpRequest::getUri() const {
	return _uri;
}

std::string HttpRequest::getHttpVersion() const {
	return _httpVersion;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const {
	return _headers;
}

std::string HttpRequest::getBody() const {
	return _body;
}

int HttpRequest::getStatusCode() const {
	return _statusCode;
}
