
#include "HttpResponse.hpp"

#include <map>
#include <string>
#include <istream>
#include <iostream>

// Responce: (class/struct? how to return)
// <version> <status> <reason-phrase>
// <headers>
// <entity-body>

HttpResponse::HttpResponse(size_t statusCode){
	statusCode++;
	statusCode--;
	makeDefaultErrorPage(statusCode);
	// Package the message
}


HttpResponse::HttpResponse(std::vector<ServerDirectives> &serverConfig, HttpRequest& request) : _response(NULL), _request(request) {
	// if http 
	if(request.getMethod() == GET)
		runGetMethod();
	else if(request.getMethod() == PUT)
		runGetMethod();
	else if(request.getMethod() == DELETE)
		runGetMethod();
	else
		runErrorMethod();
}


void HttpResponse::makeDefaultErrorPage(size_t statusCode){
	statusCode++;
}


void HttpResponse::addHeader(const std::string &header) {

}

void HttpResponse::setBody(const std::vector<uint8_t> &body) {

}

const std::istream &HttpResponse::getResponse() const {
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

void HttpResponse::runGetMethod(void){
	checkAllowedMethod();
}

void HttpResponse::runPutMethod(void){

}

void HttpResponse::runDeleteMethod(void){

}

void HttpResponse::runErrorMethod(void){

}

void HttpResponse::checkAllowedMethod(void){
	_request.
}
