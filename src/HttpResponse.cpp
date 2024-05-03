#include <map>
#include <string>
#include <istream>
#include <vector>


#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"

// Responce: (class/struct? how to return)
// <version> <status> <reason-phrase>
// <headers>
// <entity-body>

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


HttpResponse::HttpResponse(const int code) {
	makeDefaultErrorPage(code);
}

HttpResponse::HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	if (request.getHttpVersion() != "HTTP/1.1")
		makeDefaultErrorPage(505);
	else {
		if (request.getMethod() == GET)
			runGetMethod(config, request);
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

// const std::stringstream &HttpResponse::getResponse() const {
// 	return _response;
// }

void HttpResponse::runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	// Determine the server configuraiton
	std::string header = request.getHeaders().at("Host");
	std::string header_server_name = header.substr(0, header.find_first_of(':', 0));
	std::string header_port = header.substr(header.find_first_of(':', 0) + 1, header.size());
	ServerDirectives server;
	bool first = true;
	std::cout << request.getUri() << std::endl;
	for(size_t i = 0; i < config.size(); i++){
		if(header_port == config[i].listen_port && header_server_name == config[i].server_name){
			server = config[i];
			break;
		}
		else if(header_port == config[i].listen_port && first == true){
			server = config[i];
			first = false;
		}
	}
	// Determine the routing
	// int j = 0;
	// std::string uri = request.getUri();
	// for(size_t i = 0; i < server.locations.size(); i++){
	// 	if(uri.find(server.locations[i].module, 0) == 0){
			
	// 	}
	// }
	// if(isCGI(request.getUri()))
	// 	handleCGI(config, request);

}


// FILE *HttpResponse::openFileByUri(const std::string &uri, std::vector<ServerDirectives> server){
// 	std::string path;
// 	std::string query;
// 	size_t questionMark = uri.find('?');
// 	if (questionMark == std::string::npos)
// 		path = uri;
// 	else {
// 		path = uri.substr(0, questionMark);
// 		query = uri.substr(questionMark + 1);
// 	}
// 	// std::cout << "Path: " << path << "; query: " << query << std::endl;
	
// }

// bool HttpResponse::isCGI(const std::string &uri){
// 	if(std::)
// }

// void HttpResponse::handleCGI(const std::vector<ServerDirectives> &config, const HttpRequest &request){

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
