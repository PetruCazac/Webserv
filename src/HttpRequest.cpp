
#include "HttpRequest.hpp"
#include <iostream>			// std::cout
#include <string>			// std::string
#include <sstream>			// std::istringstream
#include <map>				// std::map
#include <vector>			// std::vector
#include <fstream>

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
	// return validMethods[2].method;
	throw HttpRequestParserException(HttpRequestParserException::METHOD_ERR);
}

HttpRequest::HttpRequest(std::istream &inputRequest) {
	// Printing part
	std::ostringstream output;
	output << inputRequest.rdbuf();
	std::string outputString;
	outputString = output.str();
	inputRequest.clear();
	inputRequest.seekg(0, std::ios::beg);

	std::string startLine;
	std::getline(inputRequest, startLine);
	if (startLine.empty() || startLine[0] == '\r')
		throw HttpRequestParserException(HttpRequestParserException::START_LINE_ERR);
	parseStartLine(startLine);
	findQuery();
	parseHeaders(inputRequest);
	findBoundary();
	readBody(inputRequest);
	if (!isValidContentLength())
		throw HttpRequestParserException(HttpRequestParserException::CONTENT_LENGTH_ERR);
	printHttpRequest();
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

void HttpRequest::findQuery() {
	std::string newUri;
	size_t questionMark = _uri.find('?');
	if (questionMark != std::string::npos) {
		newUri = _uri.substr(0, questionMark);
		_query = _uri.substr(questionMark + 1);
		_uri = newUri;
	}
}

void HttpRequest::findBoundary() {
	std::map<std::string, std::string>::iterator it = _headers.find("Content-Type");
	if (it != _headers.end()) {
		std::string newValue;
		size_t bound = it->second.find(';');
		if (bound != std::string::npos) {
			newValue = it->second.substr(0, bound);
			size_t boundaryPos = it->second.find("=", bound + 1) + 1;
			_boundary = it->second.substr(boundaryPos, it->second.size() - boundaryPos);
			it->second = newValue;
		}
	}
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
		size_t bodySize = _body.size();
		size_t messageSize = static_cast<size_t>(std::atol(it->second.c_str()));
		std::cout << "Actual body size: " << bodySize << std::endl;
		std::cout << "Size should be: " << messageSize << std::endl;
		if (bodySize != messageSize)
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

const std::string &HttpRequest::getQuery() const {
	return _query;
}

const std::string &HttpRequest::getHttpVersion() const {
	return _httpVersion;
}

const std::map<std::string, std::string> &HttpRequest::getHeaders() const {
	return _headers;
}

const std::string &HttpRequest::getBoundary() const {
	return _boundary;
}

const std::vector<uint8_t> &HttpRequest::getBody() const {
	return _body;
}

// Print HTTP request //
void printMethod(HttpMethods method) {
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
			std::cout << "Method: [" << methods[i].name << "]" << std::endl;
	}
}

void HttpRequest::printHttpRequest() const {
	printMethod(this->getMethod());
	std::cout << "URI: [" << _uri << "]" << std::endl;
	std::cout << "Query: [" << _query << "]" << std::endl;
	std::cout << "HTTP version: [" << _httpVersion << "]" << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); it++) {
		std::cout << "[" << it->first << ", " << it->second << "]" << std::endl;
	}
	std::cout << "Boundary: [" << _boundary << "]" << std::endl;
	std::cout << "Body: [";
	for (std::vector<uint8_t>::const_iterator it = _body.begin(); it != _body.end(); it++) {
		std::cout << *it;
	}
	std::cout << "]";
	std::cout << std::endl;
}

bool HttpRequest::isKeepAlive() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Connection");
    if (it != _headers.end()) {
        if (it->second == "keep-alive")
            return true;
    }
    return false;
}

const std::string HttpRequest::getMethodStr() const {
    static std::string methods[] = {
        "GET",
        "HEAD",
        "POST",
        "PUT",
        "TRACE",
        "OPTIONS",
        "DELETE"
    };
    return methods[_method];
}

const std::string HttpRequest::getContentLength() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
    if (it != _headers.end())
        return it->second;
    return "";
}