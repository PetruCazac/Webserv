
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"
#include <map>
#include <string>
#include <istream>
#include <vector>

// Responce: (class/struct? how to return)
// <version> <status> <reason-phrase>
// <headers>
// <entity-body>

HttpResponse::HttpResponse(const int code) {
	makeDefaultErrorPage(code);
}

HttpResponse::HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	if (request.getHttpVersion() != "HTTP/1.1")
		makeDefaultErrorPage(505);
	else {
		if (request.getMethod() == GET)
			// runGetMethod(config, request);
			;
		else if (request.getMethod() == POST)
			;
		else if (request.getMethod() == DELETE)
			;
		else
			makeDefaultErrorPage(501);
	}
}

// void HttpResponse::addHeader(const std::string &header) {

// }

void HttpResponse::makeDefaultErrorPage(const int code) {
	_response << "HTTP/1.1 "
			  << code
			  << ' '
			  << StatusCodeMap::getInstance().getStatusCodeDescription(code)
			  << "\r\n"
			  << "\r\n"
			  << setErrorBody(code);
}

std::string HttpResponse::setErrorBody(const int code) {
	std::stringstream body;
	body << "<html><body><h1>"
		 << code
		 << ' '
		 << StatusCodeMap::getInstance().getStatusCodeDescription(code)
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
	statusCodes[505] = "HTTP Version Not Supported";
}

StatusCodeMap &StatusCodeMap::getInstance() {
	static StatusCodeMap statusCodeMap;
	return statusCodeMap;
}

const std::string &StatusCodeMap::getStatusCodeDescription(const int code) {
	for (std::map<int, std::string>::iterator it = statusCodes.begin(); it != statusCodes.end(); it++) {
		if (code == it->first)
			return it->second;
	}
	throw HttpResponseExceptions(HttpResponseExceptions::CODE_NOT_EXIST);
}

// void HttpResponse::runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){

// }

// void HttpResponse::runPutMethod(void){

// }

// void HttpResponse::runDeleteMethod(void){

// }

// void HttpResponse::runErrorMethod(void){

// }

// void HttpResponse::checkAllowedMethod(void){
// 	_request.
// }
