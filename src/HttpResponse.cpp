
#include <map>
#include <string>
#include <istream>
#include <vector>
#include <unistd.h>
#include <dirent.h>

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "UtilsHttp.hpp"
#include "Config.hpp"
#include <sys/stat.h>

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
	if(isCGI(request.getUri())){
		handleCGI(server, request); // Needs to be implemented
		return;
	}
	std::string path;
	// FILE* fp = NULL;
	if(isMethodAllowed(server, "GET")){
		composeLocalUrl(server, request, path);
		if (isFile(path.c_str())){
			// fp = fopen(path.c_str(), "r");
			std::fstream fp(path);
			setBody(fp, path);
		} else if(isDirectory(path.c_str()) && checkAutoindex(server)){
			handleAutoindex(path.c_str()); // Needs to be implemented
			return;
		} else{
			makeDefaultErrorPage(404);
			return;
		}
		// if(fp == NULL){
		// 	makeDefaultErrorPage(404);
		// 	return;
		// }
	}
	
	// File pointer to be sent to the read part and sent to the client.
	// char buff[1000];
	// while(std::fgets(buff, sizeof(buff), fp) != 0){
	// 	_response << buff;
	// 	std::cout  << buff << std::endl;
	// }
}

void HttpResponse::setBody(std::fstream &file, std::string &path) {
	_response << "HTTP/1.1 200 OK\r\n";

	_response << "Content-Length: ";

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

	_response << fileSize
			  << "\r\n";

	_response << "Content-type: "
			  << MimeTypeDetector::getInstance().getMimeType(path)
			  << "\r\n";

	_response << "\r\n"; // Important: Blank line between headers and body

	std::string line;
    while (std::getline(file, line)) {
        _response << line << std::endl;
    }
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

void HttpResponse::handleCGI(const ServerDirectives &config, const HttpRequest &request){
	std::cout << config.name << std::endl;
	std::cout << request.getUri() << std::endl;
}

void HttpResponse::handleAutoindex(const char* path){
	DIR* dir = opendir(path);
	if (dir != NULL) {
		_response << "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>";
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			_response << "<li>" << entry->d_name << "</li>";
		}
		_response << "</ul></body></html>";
		closedir(dir);
	} else
		_response << "<html><head><title>Error</title></head><body><h1>Error: Unable to open directory</h1></body></html>";
}

const std::stringstream &HttpResponse::getResponse(){
	if(_response.str().empty())
		makeDefaultErrorPage(500);
	return _response;
}

// const std::string &HttpResponse::getResponse(){
// 	const std::string& str = _response.str();
// 	if(str.empty()){
// 		makeDefaultErrorPage(500);
// 		const std::string& errorStr = _response.str();
// 		return errorStr;
// 	}
// 	return str;
// }

// ------------------------ Helper Functions -------------------------

bool HttpResponse::checkAutoindex(ServerDirectives& server){
	if(!server.locations.empty() && server.locations[0].autoindex == "on")
		return true;
	else if(server.autoindex == "on")
		return true;
	return false;
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
