
#include "HttpRequest.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <cstdlib>
#include <map>

s_httpRequest parseHttpRequest(std::string &inputRequest) {
	s_httpRequest httpRequest;
	e_errors err = NO_ERR;

	httpRequest.statusCode = 200;
	size_t size = inputRequest.size();
	size_t crlfStart = inputRequest.find("\r\n");
	err = parseStartLine(httpRequest, inputRequest, crlfStart);
	if (err != NO_ERR) {
		return error(httpRequest.statusCode, err), httpRequest;
	}
	size_t crlfHeader = crlfStart + 2;
	err = parseHeaders(httpRequest, inputRequest, crlfHeader, size);
	if (err != NO_ERR) {
		return error(httpRequest.statusCode, err), httpRequest;
	}
	size_t crlfBody = inputRequest.find("\r\n\r\n");
	err = parseBody(httpRequest, inputRequest, crlfBody, size);
	if (err != NO_ERR) {
		return error(httpRequest.statusCode, err), httpRequest;
	}
	return httpRequest;
}

e_errors parseStartLine(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfStart) {
	if (crlfStart != std::string::npos) {
		std::string startLine = inputRequest.substr(0, crlfStart);
		size_t methodEnd = startLine.find(' ');
		if (methodEnd != std::string::npos) {
			httpRequest.method = startLine.substr(0, methodEnd);
			size_t urlEnd = startLine.find(' ', methodEnd + 1);
			if (urlEnd != std::string::npos) {
				httpRequest.uri = startLine.substr(methodEnd + 1, urlEnd - methodEnd - 1);
				if (httpRequest.uri.size() == 0) {
					return URI_ERR;
				}
			} else {
				return URI_ERR;
			}
			httpRequest.httpVersion = startLine.substr(urlEnd + 1, crlfStart);
			if (!ifHttpVersion(httpRequest.httpVersion)) {
				return HTTPVERSION_ERR;
			}
		} else {
			return METHOD_ERR;
		}
	}
	else {
		return METHOD_ERR;
	}
	if (!ifMethod(httpRequest.method)) {
		return METHOD_ERR;
	}
	return NO_ERR;
}

e_errors parseHeaders(s_httpRequest &httpRequest, std::string &inputRequest, size_t &crlfHeader, size_t size) {
	while (crlfHeader != std::string::npos && crlfHeader < size) {
		size_t lineEnd = inputRequest.find("\r\n", crlfHeader);
		if (lineEnd != std::string::npos && lineEnd - crlfHeader > 0) {
			std::string line = inputRequest.substr(crlfHeader, lineEnd - crlfHeader);
			size_t colon = line.find(':');
			if (colon != std::string::npos) {
				httpRequest.headers.insert(std::make_pair(line.substr(0, colon), line.substr(colon + 2)));
			} else {
				return HEADER_ERR;
			}
			crlfHeader = lineEnd + 2;
		} else {
			break;
		}
	}
	if (httpRequest.headers.size() == 0) {
		return HEADER_ERR;
	}
	return NO_ERR;
}

e_errors parseBody(s_httpRequest &httpRequest, std::string &inputRequest, size_t crlfBody, size_t size) {
	if (crlfBody != std::string::npos) {
		httpRequest.body = inputRequest.substr(crlfBody + 4, size - crlfBody - 6);
	} else {
		;
	}
	if (!ifValidBody(httpRequest)) {
		return CONTENT_LENGTH_ERR;
	}
	return NO_ERR;
}

bool ifMethod(std::string &method) {
	const std::array<std::string, 7> validMethods = {"GET", "HEAD", "POST", "PUT", "TRACE", "OPTIONS", "DELETE"};
	std::array<std::string, 1>::const_iterator it = std::find(validMethods.begin(), validMethods.end(), method);
	if (it == validMethods.end()) {
		return false;
	}
	return true;
}

bool ifHttpVersion(std::string &httpVersion) {
	const std::array<std::string, 5> validHttpVersions = {"HTTP/0.9", "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3"};
	std::array<std::string, 1>::const_iterator it = std::find(validHttpVersions.begin(), validHttpVersions.end(), httpVersion);
	if (it == validHttpVersions.end()) {
		return false;
	}
	return true;
}

bool ifValidBody(s_httpRequest &httpRequest) {
	std::map<std::string, std::string>::iterator it = httpRequest.headers.find("Content-Length");
	if (it != httpRequest.headers.end()) {
		if (httpRequest.body.length() + 2 != static_cast<size_t>(std::atol(it->second.c_str()))) {
			return false;
		}
	}
	return true;
}

void error(int &statusCode, e_errors err) {
	statusCode = 400;
	std::cerr << "Error: bad HTTP request";
	switch (err)
	{
		case METHOD_ERR: std::cerr << ": method" << std::endl; break;
		case URI_ERR: std::cerr << ": URI" << std::endl; break;
		case HTTPVERSION_ERR: std::cerr << ": HTTP version" << std::endl; break;
		case HEADER_ERR: std::cerr << ": header" << std::endl; break;
		// case BODY_ERR: std::cerr << ": body" << std::endl; break;
		case CONTENT_LENGTH_ERR: std::cerr << ": content-length mismatch" << std::endl; break;
		default: std::cerr << std::endl; break;
	}
}
