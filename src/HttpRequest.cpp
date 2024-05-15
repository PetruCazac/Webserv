
#include "HttpRequest.hpp"
#include <iostream>			// std::cout
#include <string>			// std::string
#include <sstream>			// std::istringstream
#include <map>				// std::map
#include <vector>			// std::vector

void printHttpRequest(HttpRequest &httpRequest);

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

HttpRequest::HttpRequest(std::istream &inputRequest) {
	std::string startLine;
	std::getline(inputRequest, startLine);
	if (startLine.empty() || startLine[0] == '\r')
		throw HttpRequestParserException(HttpRequestParserException::START_LINE_ERR);
	parseStartLine(startLine);
	parseHeaders(inputRequest);
	readBody(inputRequest);
	if (!isValidContentLength())
		throw HttpRequestParserException(HttpRequestParserException::CONTENT_LENGTH_ERR);
	printHttpRequest(*this);
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

void HttpRequest::parseHeaders(std::istream &inputRequest) {
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

void HttpRequest::readBody(std::istream &inputRequest) {
	inputRequest >> std::noskipws;
	std::vector<uint8_t> body((std::istream_iterator<uint8_t>(inputRequest)), std::istream_iterator<uint8_t>());
	_body = body;
}

bool HttpRequest::isValidHttpVersion() const {
	static std::string validHttpVersions[5] = {"HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"};
	for (int i = 0; i != 5; i++) {
		if (_httpVersion == validHttpVersions[i])
			return true;
	}
	return false;
}

bool HttpRequest::isValidContentLength() const {
	std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
	if (it != _headers.end()) {
		if (_body.size() != static_cast<size_t>(std::atol(it->second.c_str())))
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

const std::vector<uint8_t> &HttpRequest::getBody() const {
	return _body;
}


// To be put in a class ////////////////////////////////////////

void printMethod(HttpMethods method) { // delete
	Methods methods[] = {
		{"GET", GET},
		{"HEAD", HEAD},
		{"POST", POST},
		{"PUT", PUT},
		{"TRACE", TRACE},
		{"OPTIONS", OPTIONS},
		{"DELETE", DELETE}
	};
	const int size = sizeof(methods) / sizeof(methods[0]);
	for (int i = 0; i != size; i++) {
		if (method == methods[i].method)
			std::cout << "[" << methods[i].name << "]" << std::endl;
	}
}

void printHttpRequest(HttpRequest &httpRequest) { // delete
	printMethod(httpRequest.getMethod());
	std::cout << "[" << httpRequest.getUri() << "]" << std::endl;
	std::cout << "[" << httpRequest.getHttpVersion() << "]" << std::endl;
	std::map<std::string, std::string> headers = httpRequest.getHeaders();
	if (headers.size() == 0)
		std::cout << "[no headers]" << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
		std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
	}
	std::vector<uint8_t> body = httpRequest.getBody();
	if (body.size() == 0)
		std::cout << "[no body]" << std::endl;
	else {
		std::cout << "[";
		for (std::vector<uint8_t>::iterator it = body.begin(); it != body.end(); it++) {
			std::cout << *it;
		}
		std::cout << "]";
		std::cout << std::endl;
	}
}

bool HttpRequest::isKeepAlive() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Connection");
    if (it != _headers.end()) {
        if (it->second == "keep-alive")
            return true;
    }
    return false;
}