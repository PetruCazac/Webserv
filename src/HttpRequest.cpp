
#include "HttpRequest.hpp"
#include <iostream>			// std::cout
#include <string>			// std::string
#include <sstream>			// std::istringstream
#include <map>				// std::map

HttpRequest::HttpRequest(std::istringstream &inputRequest) {
	std::string startLine;
	std::getline(inputRequest, startLine);
	if (startLine.empty() || startLine[0] == '\r')
		throw HttpRequestParserException(HttpRequestParserException::START_LINE_ERR);
	parseStartLine(startLine);
	parseHeaders(inputRequest);
	parseBody(inputRequest);
	if (!validContentLength())
		throw HttpRequestParserException(HttpRequestParserException::CONTENT_LENGTH_ERR);
	_statusCode = 200;
}

void HttpRequest::parseStartLine(std::string &line) {
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

void HttpRequest::parseHeaders(std::istringstream &inputRequest) {
	std::string line;
	while (std::getline(inputRequest, line)) {
		if (line[0] == '\r')
			break;
		size_t colon = line.find(':');
		line.erase(line.end() - 1);
		_headers.insert(std::make_pair(line.substr(0, colon), line.substr(colon + 2)));
	}
}

void HttpRequest::parseBody(std::istringstream &inputRequest) {
	for (std::string line; getline(inputRequest, line);) {
		if (line[1] == '\r')
			continue;
		_body.append(line);
	}
}

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

bool HttpRequest::validContentLength() const {
	std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
	if (it != _headers.end()) {
		if (_body.length() + 1 != static_cast<size_t>(std::atol(it->second.c_str())))
			return false;
	}
	return true;
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
