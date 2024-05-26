
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
	makeDefaultErrorResponse(code);
}

HttpResponse::HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	if (request.getHttpVersion() != "HTTP/1.1")
		makeDefaultErrorResponse(505);
	else {
		if (request.getMethod() == GET) {
			runGetMethod(config, request);
		}
		else if (request.getMethod() == POST)
			;
		else if (request.getMethod() == DELETE)
			;
		else
			makeDefaultErrorResponse(501);
	}
}

void HttpResponse::makeDefaultErrorResponse(const int code) {
	std::string errorBody = getErrorBody(code);
	_response << "HTTP/1.1 " << code << ' ' << StatusCodeMap::getInstance().getStatusCodeDescription(code)
			  << "\r\n" << "Content-Type: text/html" << "\r\n"
			  << "Content-Length: " << errorBody.size() << "\r\n\r\n"
			  << errorBody;
}

std::string HttpResponse::getErrorBody(const int code) {
	std::stringstream body;
	body << "<html><body><h1>"
		 << code
		 << ' '
		 << StatusCodeMap::getInstance().getStatusCodeDescription(code)
		 << "</h1></body></html>";
	return body.str();
}

void HttpResponse::runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	std::string header_server_name = request.getHeaders().at("Host").substr(0, request.getHeaders().at("Host").find_first_of(':', 0));
	ServerDirectives server;
	bool first = true;
	for(size_t i = 0; i < config.size(); i++){
		if(header_server_name == config[i].server_name){
			server = config[i];
			break;
		}
		else if(first == true){
			server = config[i];
			first = false;
		}
	}
	LocationDirectives location;
	findLocationUri(server.locations, request.getUri(), location);
	server.locations.clear();
	if(!location.module.empty())
		server.locations.push_back(location);
	if(isCGI(request.getUri())){
		handleCGI(server, request); // Needs to be implemented
		return;
	}
	std::string path;
	if(isMethodAllowed(server, "GET")){
		composeLocalUrl(server, request, path);
		if (isFile(path.c_str())){
			readFile(path);
			setResponse();
		} else if(isDirectory(path.c_str()) && checkAutoindex(server)){
			handleAutoindex(path.c_str());
		} else{
			makeDefaultErrorResponse(404);
		}
	}
	else {
		makeDefaultErrorResponse(405);
	}
}

void HttpResponse::readFile(std::string &path) {
	std::ifstream file(path.c_str());
	if (!file.is_open())
		makeDefaultErrorResponse(500);
	else {
		_contentType = MimeTypeDetector::getInstance().getMimeType(path);
		std::stringstream body;
		body << file.rdbuf();
		_body = body.str();
	}
}

void HttpResponse::setHeader(const std::string &header, const std::string &value) {
	_headers[header] = value;
}

void HttpResponse::setResponse() {
	_response << "HTTP/1.1 200 OK\r\n";
	_response << "Content-Type: " << _contentType << "\r\n";
	_response << "Content-Length: " <<_body.length() << "\r\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++) {
		_response << it->first << ": " << it->second << "\r\n";
	}
	_response << "\r\n" << _body;
}

void cleanPath(std::string& path, const std::string& serverRoot){
	std::stringstream ss(path);
	std::string segment;
	std::vector<std::string> uriSegments;
	while(std::getline(ss, segment, '/')){
		if(!segment.empty())
			uriSegments.push_back(segment);
	}
	if(!uriSegments.empty() && uriSegments[0] == serverRoot){
		std::string withoutRoot;
		for(size_t i = 1; i < uriSegments.size(); i++)
			withoutRoot.append("/" + uriSegments[i]);
		path = withoutRoot;
	}
}

void HttpResponse::composeLocalUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path){
	path = request.getUri();
	cleanPath(path, server.root);
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
			uriSegments.push_back(segment + '/');
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
				locationMethods.push_back(segment + '/');
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
	std::string scriptPath = request.getUri();
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        makeDefaultErrorResponse(500);
        return;
    }
    pid_t pid = fork();
    if (pid == -1) {
        makeDefaultErrorResponse(500);
        return;
    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        // if (request.getMethod() == POST) {
        //     std::ofstream cgiInput("cgi_input.txt");
        //     cgiInput << request.getBody();
        //     cgiInput.close();
        //     int fd = open("cgi_input.txt", O_RDONLY);
        //     dup2(fd, STDIN_FILENO);
        //     close(fd);
        // }
        setCgiEnvironment(request, scriptPath);

        chdir(getFilePath(scriptPath).c_str());
        execl(scriptPath.c_str(), scriptPath.c_str(), NULL);
        exit(1); 
    } else {
        close(pipefd[1]); 
        fcntl(pipefd[0], F_SETFL, O_NONBLOCK); 
        _cgi_pipe_fd = pipefd[0];
        _cgi_pid = pid;
        _is_cgi_response = true;
    }
}

std::string getPath(const char *path){
	std::string str(path);
	size_t start = str.find("/", 0);
	std::string withoutRoot(str, start, str.size());
	if(*(withoutRoot.end() - 1) != '/')
		withoutRoot.append("/");
	return withoutRoot.c_str();
}

void HttpResponse::handleAutoindex(const char* path){
	DIR* dir = opendir(path);
	_response << "HTTP/1.1 200 OK\r\n";
	_response << "Content-Length: ";
	std::string str;
	if (dir != NULL) {
		str = str + "<html><head><title>Directory Listing</title></head><body><h1>Directory Listing</h1><ul>" + "\r\n";
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
			str = str + "<li> " + "<a href=\"" + getPath(path) + entry->d_name + "\">" + entry->d_name + "</a>"+ "</li>" + "\r\n";
		str = str + "</ul></body></html>" + "\r\n";
		closedir(dir);
	} else
		str = str + "<html><head><title>Error</title></head><body><h1>Error: Unable to open directory</h1></body></html>" + "\r\n";
	_response << str.size() << "\r\n";
	_response << "Content-type: text/html\r\n";
	_response << "\r\n";
	_response << str;
}

const std::stringstream &HttpResponse::getResponse() const {
	return _response;
}

// ------------------------ Helper Functions -------------------------

bool HttpResponse::checkAutoindex(ServerDirectives& server){
	if(!server.locations.empty()){
		if(server.locations[0].autoindex == "on")
			return true;
		else
			return false;
	}
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

int HttpResponse::getCgiPipeFd() const {
    return _cgi_pipe_fd;
}

pid_t HttpResponse::getCgiPid() const {
    return _cgi_pid;
}

void HttpResponse::appendCgiOutput(const std::string &data) {
    _cgi_output.append(data);
}

void HttpResponse::finalizeCgiResponse() {
    _response << "HTTP/1.1 200 OK\r\n";
    _response << "Content-Type: text/html\r\n";
    _response << "Content-Length: " << _cgi_output.size() << "\r\n";
    _response << "\r\n";
    _response << _cgi_output;
}

bool HttpResponse::isCgiResponse() const {
    return _is_cgi_response;
}

void HttpResponse::setCgiEnvironment(const HttpRequest& request, const std::string& scriptPath) {
    setenv("REQUEST_METHOD", request.getMethodStr().c_str(), 1);
    setenv("SCRIPT_NAME", scriptPath.c_str(), 1);
    setenv("PATH_INFO", scriptPath.c_str(), 1);

    std::string queryString = request.getQueryString();
    if (!queryString.empty()) {
        setenv("QUERY_STRING", queryString.c_str(), 1);
    }

    std::string contentLength = request.getContentLength();
    if (!contentLength.empty()) {
        setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
    }
}

std::string HttpResponse::getFilePath(const std::string& scriptPath) const {
    size_t lastSlash = scriptPath.find_last_of('/');
    if (lastSlash == std::string::npos) {
        return ".";
    }
    return scriptPath.substr(0, lastSlash);
}

void HttpResponse::setCgiResponse(const int cgi_pipe_fd, pid_t cgi_pid, const bool is_cgi_response) {
    _cgi_pipe_fd = cgi_pipe_fd;
    _cgi_pid = cgi_pid;
    _is_cgi_response = is_cgi_response;
}