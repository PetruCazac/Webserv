#include <map>
#include <string>
#include <istream>
#include <vector>
#include <unistd.h>

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Config.hpp"
#include <sys/stat.h>

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
	std::string header = request.getHeaders().at("Host");
	std::string header_server_name = header.substr(0, header.find_first_of(':', 0));
	std::string header_port = header.substr(header.find_first_of(':', 0) + 1, header.size());
	ServerDirectives server;
	bool first = true;
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

	LocationDirectives location;
	findLocationUri(server.locations, request.getUri(), location);
	server.locations.clear();
	if(!location.module.empty()){
		server.locations.push_back(location);
	}
	// if(isCGI(request.getUri()))
	// 	handleCGI(server, request);
	std::string path;
	FILE* fp = NULL;
	if(isMethodAllowed(server, "GET")){
		composeLocalUrl(server, request, path);
		if (isFile(path.c_str())){
			fp = fopen(path.c_str(), "r");
		} else if(isDirectory(path.c_str()) && checkAutoindex(server)){
			treatAutoindex(path.c_str());
			return;
		} else{
			makeDefaultErrorPage(404);
			return;
		}
		if(fp == NULL){
			makeDefaultErrorPage(404);
			return;
		}
	}
	char buff[1000];

	while(std::fgets(buff, sizeof(buff), fp) != 0)
		_response << buff;
	std::cout  << _response << std::endl;

}

bool HttpResponse::checkAutoindex(ServerDirectives& server){
	if(!server.locations.empty() && server.locations[0].autoindex == "on")
		return true;
	else if(server.autoindex == "on")
		return true;
	return false;
}

void HttpResponse::treatAutoindex(const char* path){
	// not implemented
	std::cout  << path << std::endl;
}

void HttpResponse::composeLocalUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path){
	path = request.getUri();
	struct stat fileInfo;
	if(!server.locations.empty()){
		if(!server.locations[0].root.empty()){
			size_t pos;
			pos = path.find(server.locations[0].module);
			if(pos != std::string::npos)
				path = server.locations[0].root + path.substr(pos, path.size());
		} else{
			path = server.root + path;
		}
		if(stat(path.c_str(), &fileInfo) == -1){
			if(!server.locations[0].index.empty()){
				path = server.locations[0].root + '/' + server.locations[0].index;
				return;
			}
			return path.clear();;
		}
		return;
	}else{
		path = server.root + path;
		if(stat(path.c_str(), &fileInfo) == -1){
			if(!server.index.empty()){
				path = server.root + '/' + server.index;
				return;
			}
			return path.clear();
		}
		return;
	}
	return path.clear();
}

bool HttpResponse::isDirectory(const char* path){
	struct stat fileInfo;
	if (stat(path, &fileInfo) != 0) {
		return false;
	}
	return S_ISDIR(fileInfo.st_mode);
}

bool HttpResponse::isFile(const char* path){
	struct stat fileInfo;
	if (stat(path, &fileInfo) != 0) {
		return false;
	}
	return S_ISREG(fileInfo.st_mode);
}

bool	HttpResponse::isMethodAllowed(const ServerDirectives& server, const std::string method){
	for(size_t i = 0; !server.locations.empty() && i < server.locations[0].allow.size(); i++){
		if(server.locations[0].allow[i] == method)
			return true;
	}
	for(size_t i = 0; i < server.allow.size(); i++){
		if(server.allow[i] == method)
			return true;
	}
	return false;
}

bool HttpResponse::isCGI(const std::string& uri){
	std::string cgiBin = "/cgi_bin";
	if(uri.find(cgiBin) == 0)
		return true;
	return false;
}

void HttpResponse::findLocationUri(const std::vector<LocationDirectives>& locations, const std::string& uri, LocationDirectives& location){
	std::stringstream ss(uri);
	std::string segment;
	std::vector<std::string> uriSegments;
	while(std::getline(ss, segment, '/')){
		if(!segment.empty())
			uriSegments.push_back('/' + segment);
	}
	if(uriSegments.empty() || locations.empty())
		return;
	std::vector<LocationDirectives> matchingLocations;
	std::vector<std::string> locationMethods;
	int count = 0;
	int countActual = 0;

	for (size_t i = 0; i < locations.size(); i++){
		std::stringstream ss(locations[i].module);
		while(std::getline(ss, segment, '/')){
			if(!segment.empty())
				locationMethods.push_back('/' + segment);
		}
		for(size_t j = 0; j < locationMethods.size() && j < uriSegments.size(); j++){
			if(uriSegments[j] == locationMethods[j])
				count++;
			else
				break;
		}
		if(count > countActual){
			countActual = count;
			location = locations[i];
			count = 0;
		}
		locationMethods.clear();
	}
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
