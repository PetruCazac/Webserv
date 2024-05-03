
#include "../headers/HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "config/Config.hpp"
#include <map>
#include <string>
#include <istream>
#include <vector>

// Responce: (class/struct? how to return)
// <version> <status> <reason-phrase>
// <headers>
// <entity-body>

HttpResponse::HttpResponse(const int code) {
	std::string body = setErrorBody(code);
	_response << "HTTP/1.1 "
			  << code
			  << ' '
			  << StatusCodeMap::getInstance().getStatusCode(code)
			  << "\r\n"
			  << "\r\n"
			  << body;
}

HttpResponse::HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	
}

// void HttpResponse::addHeader(const std::string &header) {

// }

std::string HttpResponse::setErrorBody(const int code) {
	std::stringstream body;
	body << "<html><body><h1>"
		 << code
		 << ' '
		 << StatusCodeMap::getInstance().getStatusCode(code)
		 << "</h1></body></html>";
	return body.str();
}

const std::stringstream &HttpResponse::getResponse() const {
	return _response;
}

StatusCodeMap::StatusCodeMap() {
	statusCodes[200] = "OK";
	statusCodes[201] = "Created";
	statusCodes[202] = "Accepted";
	statusCodes[204] = "No Content";
	statusCodes[304] = "Not Modified";
	statusCodes[400] = "Bad Request";
	statusCodes[401] = "Unauthorized";
	statusCodes[403] = "Forbidden";
	statusCodes[404] = "Not Found";
	statusCodes[405] = "Method Not Allowed";
	statusCodes[409] = "Conflict";
	statusCodes[410] = "Gone";
	statusCodes[500] = "Internal Server Error";
	statusCodes[501] = "Not Implemented";
	statusCodes[502] = "Bad Gateway";
	statusCodes[503] = "Service Unavailable";
}

StatusCodeMap &StatusCodeMap::getInstance() {
	static StatusCodeMap statusCodeMap;
	return statusCodeMap;
}

const std::string &StatusCodeMap::getStatusCode(const int code) {
	for (std::map<int, std::string>::iterator it = statusCodes.begin(); it != statusCodes.end(); it++) {
		if (code == it->first)
			return it->second;
	}
	throw HttpResponseExceptions(HttpResponseExceptions::ERR);
}
