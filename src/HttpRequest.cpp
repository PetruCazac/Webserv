
#include "HttpRequest.hpp"
#include <iostream>			// std::cout
#include <string>			// std::string
#include <sstream>			// std::istringstream
#include <map>				// std::map

struct Methods {
	std::string name;
	HttpMethods method;
};

static HttpMethods methodToEnum(const std::string &method) {
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
	throw HttpRequestParserException(HttpRequestParserException::METHOD_ERR);
}

HttpRequest::HttpRequest(std::istringstream &inputRequest) {
	std::string startLine;
	std::getline(inputRequest, startLine);
	if (startLine.empty() || startLine[0] == '\r')
		throw HttpRequestParserException(HttpRequestParserException::START_LINE_ERR);
	parseStartLine(startLine);
	parseHeaders(inputRequest);
	std::ostringstream body;
	body << inputRequest.rdbuf();
	_body = body.str();
	if (!isValidContentLength())
		throw HttpRequestParserException(HttpRequestParserException::CONTENT_LENGTH_ERR);
}

void HttpRequest::parseStartLine(const std::string &line) {
	std::string method;
	std::istringstream startLine(line);
	startLine >> method;
	startLine >> _uri;
	startLine >> _httpVersion;
	_method = methodToEnum(method);
	if (_uri.size() == 0 || _uri[0] != '/')
		throw HttpRequestParserException(HttpRequestParserException::URI_ERR);
	if (!isValidHttpVersion())
		throw HttpRequestParserException(HttpRequestParserException::HTTP_VERSION_ERR);
}

void HttpRequest::parseHeaders(std::istringstream &inputRequest) {
	std::string line;
	while (std::getline(inputRequest, line)) {
		if (line.size() != 0 && line[0] == '\r')
			break;
		size_t colon = line.find(':');
		if (colon == std::string::npos)
			throw HttpRequestParserException(HttpRequestParserException::HEADER_ERR);
		line.erase(line.end() - 1);
		_headers.insert(std::make_pair(line.substr(0, colon), line.substr(colon + 2)));
	}
}

bool HttpRequest::isValidHttpVersion() const {
	std::string validHttpVersions[5] = {"HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"};
	for (int i = 0; i != 5; i++) {
		if (_httpVersion == validHttpVersions[i])
			return true;
	}
	return false;
}

bool HttpRequest::isValidContentLength() const {
	std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
	if (it != _headers.end()) {
		if (_body.length() != static_cast<size_t>(std::atol(it->second.c_str())))
			return false;
	}
	return true;
}

HttpMethods HttpRequest::getMethod() const {
	return _method;
}

const std::string &HttpRequest::getUri() const {
	return _uri;
}

const std::string &HttpRequest::getHttpVersion() const {
	return _httpVersion;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
	return _headers;
}

const std::string &HttpRequest::getBody() const {
	return _body;
}
