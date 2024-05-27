
#include <map>
#include <string>
#include <istream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "UtilsHttp.hpp"
#include "Config.hpp"

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
			runPostMethod(config, request);
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

// --------------------------------- POST Method --------------------------------------

bool HttpResponse::storeFormData(const std::string& formData, std::string& path) {
	std::stringstream time_int;
	struct stat fileInfo;
	time_int << time(NULL);
	stat(path.c_str(), &fileInfo);
	if(S_ISDIR(fileInfo.st_mode)){
		std::string filename = path + "/" + "form_submission_" + time_int.str() + ".txt";
		std::ofstream outFile(filename.c_str(), std::ios_base::app);
		if (!outFile) {
			LOG_ERROR("Error opening file for writing: " + filename);
			return false;
		}
		outFile << formData;
		outFile.close();
		return true;
	} else {
		std::ofstream file(path.c_str(), std::ios_base::app);
		if(file.is_open()){
			file << formData;
			file.close();
			return true;
		} else {
			LOG_ERROR("POST Request file could not be created for the requested path: " + path);
			return false;
		}
	}
	return false;
}

void HttpResponse::composePostUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path){
	path = request.getUri();
	cleanPath(path, server.root);
	struct stat fileInfo;
	if(!server.locations.empty()){
		if(path.find(server.locations[0].module, 0) == 0){
			path = server.locations[0].root + path;
			if(stat(path.c_str(), &fileInfo) != -1){
				LOG_INFO("POST Request will be saved at: " + path);
				return;
			}
		}
	} else {
		if(path.find(server.post_dir, 0) == 0){
			path = server.root + path;
			int i = stat(path.c_str(), &fileInfo);
			if(i != -1){
				LOG_INFO("POST Request will be saved at: " + path);
				return;
			}
		}
	}
	size_t start = path.find_last_of('/', path.size());
	if(start != std::string::npos){
		std::string lastElement = path.substr(start + 1, path.size() - start - 1);
		if(MimeTypeDetector::getInstance().getMimeType(lastElement) != "application/octet-stream"){
			if(stat(path.substr(0, start).c_str(), &fileInfo) != -1){
				if(S_ISDIR(fileInfo.st_mode)){
					LOG_INFO("POST Request will be saved at a new file: " + path);
					return;
				}
				LOG_ERROR("The requested path is not valid: " + path);
			}
		}
	}
	path.clear();
	return;
}

std::string HttpResponse::createFilename(std::string& path, std::map<std::string, std::string>& metadata){
	std::string filePath;
	std::string fileName = metadata.find("filename")->second;
	size_t last = path.find_last_of('/');
	if(last == std::string::npos)
		return "";
	if(MimeTypeDetector::getInstance().getMimeType(path.substr(last + 1, path.size() - last - 1)) != "application/octet-stream")
		filePath = path.substr(0, last);
	else
		filePath = path;

	struct stat fileInfo;
	if(stat(filePath.c_str(), &fileInfo))
		return "";
	std::string filetype = MimeTypeDetector::getInstance().getMimeType(fileName);
	if(filetype == "application/octet-stream")
		return "";
	if(filetype != metadata.find("Content-Type:")->second)
		fileName += MimeTypeDetector::getInstance().getExtension(filetype);
	if(S_ISDIR(fileInfo.st_mode)){
		filePath = filePath + '/' + fileName;
		return filePath;
	} else
		return "";
}

bool HttpResponse::handlePackage(std::string& package, std::string& path){
	size_t endMetadata = package.find("\r\n\r\n");
	for(size_t i = 0; i < endMetadata; i++){
		if (package[i] == '\r' && (i + 1 < endMetadata) && package[i + 1] == '\n') {
			package[i] = ';';
			package[i + 1] = ';';
			++i;
		}
	}

	std::stringstream packageStream(package.substr(0, endMetadata));
	std::string temp;
	std::map<std::string, std::string> metadata;


	while(std::getline(packageStream, temp, ';')){
		if(temp.empty())
			continue;
		if(temp.find("Content-Disposition: ") != std::string::npos){
			size_t start = temp.find("Content-Disposition: ") + 21;
			size_t end = temp.find(";");
			if(end == std::string::npos)
				end = temp.size();
			metadata["Content-Disposition:"] = temp.substr(start, end - start);
		} else if(temp.find("Content-Type: ") != std::string::npos){
			size_t start = temp.find("Content-Type: ") + 14;
			size_t end = temp.find(";");
			if(end == std::string::npos)
				end = temp.size();
			metadata["Content-Type:"] = temp.substr(start, end - start);
		} else {
			size_t start = 1;
			size_t end = temp.find("=");
			std::string first = temp.substr(start, end - start);
			start = end + 1;
			end = temp.find(";");
			if(end == std::string::npos)
				end = temp.size();
			std::string second = temp.substr(start, end - start);
			if(second[0] == '\"' && second[second.size() - 1] == '\"')
				second = second.substr(1, second.size() - 2);
			metadata[first] = second;
		}
	}
	if(metadata.find("Content-Disposition:")->second != "form-data"){
		makeDefaultErrorResponse(415);
		return false;
	}
	if(metadata.find("name")->second == "file"){
		std::string information = package.substr(endMetadata + 4, package.size() - endMetadata - 4);
		if(information.empty() || metadata["Content-Type:"] == "application/octet-stream")
			return true;
		std::string filename = createFilename(path, metadata);
		if(filename.empty()){
			makeDefaultErrorResponse(400);
			return false;
		}
		struct stat fileInfo;
		if(stat(filename.c_str(), &fileInfo) == 0){
			LOG_INFO("File already exists, overwriting not allowed at the: " + filename);
			return true;
		}
		std::ofstream newFile(filename.c_str());
		if(newFile.is_open()){
			newFile << information;
			newFile.close();
		}
	}else{
		storeFormData(metadata.find("name")->second + "=" + package.substr(endMetadata + 4, package.size() - endMetadata - 4), path);
	}
	return true;
}

void HttpResponse::handleMultipart(const HttpRequest& request, std::string& path){
	std::string boundary = "--" + request.getBoundary();
	std::vector<uint8_t> body = request.getBody();
	std::stringstream ss;
	for(size_t i = 0; i < body.size(); i++)
		ss << static_cast<char>(body[i]);
	std::string package;
	std::string temp;
	std::vector<std::string> packages;
	size_t startPos = 0;
	startPos = ss.str().find(boundary, startPos) + boundary.size();
	size_t endPos = ss.str().find(boundary, startPos);

	while(endPos != std::string::npos){
		std::string package = ss.str().substr(startPos, endPos - startPos);
		startPos = endPos + boundary.size();
		if(package.empty())
			break;
		packages.push_back(package);
		endPos = ss.str().find(boundary, startPos);
	}
	for(size_t i = 0; i < packages.size(); i++){
		if(!handlePackage(packages[i], path))
			return;
	}
	makeDefaultErrorResponse(200);
}

void HttpResponse::handleUriEncoding(const HttpRequest& request, std::string& path){
	if(!storeFormData(urlDecode(request.getBody()), path))
		makeDefaultErrorResponse(400);
}

void HttpResponse::runPostMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	ServerDirectives server;
	chooseServerConfig(config, request, server);
	if(isCGI(request.getUri())){
		handleCGI(server, request); // Needs to be implemented
		return;
	}
	// Check if there is a submit folder where it will be written the file.
	std::string path;
	if(isMethodAllowed(server, "POST")){
		composePostUrl(server, request, path);
		if(!path.empty()){
	
			std::string contentType(request.getHeaders().find("Content-Type")->second);
			if(contentType.empty()){
				LOG_ERROR("No Content-Type found.");
				makeDefaultErrorResponse(400);
				return;
			}
			if(contentType == "multipart/form-data")
				handleMultipart(request, path);
			else if(contentType == "application/x-www-form-urlencoded"){
				if(!storeFormData(urlDecode(request.getBody()), path))
					makeDefaultErrorResponse(400);
				else
					makeDefaultErrorResponse(200);
			}else
				makeDefaultErrorResponse(400);
		} else
			makeDefaultErrorResponse(400);
	} else
		makeDefaultErrorResponse(405);
}

// --------------------------------- GET Method --------------------------------------

void HttpResponse::runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	ServerDirectives server;
	chooseServerConfig(config, request, server);
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
	}else
		makeDefaultErrorResponse(405);
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

void HttpResponse::chooseServerConfig(const std::vector<ServerDirectives>& config, const HttpRequest &request, ServerDirectives& server){
	std::string header_server_name = request.getHeaders().at("Host").substr(0, request.getHeaders().at("Host").find_first_of(':', 0));
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

const std::string getRoot(const ServerDirectives& config){
    if(!config.locations.empty()){
        return config.locations[0].root;
    } else
        return config.root;
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
        std::cout << "Entering child process: \n\n";
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
        // setCgiEnvironment(request, scriptPath);
        // chdir(getFilePath(scriptPath).c_str());
        char *argv[] = { const_cast<char*>(scriptPath.c_str()), NULL };
        // if (chdir(getFilePath(scriptPath, getRoot(config)).c_str()) == -1){
        if (chdir(getRoot(config).c_str()) == -1){
            LOG_ERROR("CHDIR_CALL_FAILED");
            exit(2);
        }
        LOG_DEBUG(scriptPath.c_str());
        execv(scriptPath.c_str(),argv);
        perror("execv");
        exit(1); 
    } else {
        // std::cout << "Script path:" <<scriptPath.c_str() << "FilePath" << getFilePath(scriptPath).c_str();
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

int hexCharToInt(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else {
		// Could throw an exception
		LOG_ERROR("Body decoding error");
		return 0;
	}
}

std::string HttpResponse::urlDecode(const std::vector<uint8_t>& bodyString) {
	std::ostringstream decoded;
	for (size_t i = 0; i < bodyString.size(); ++i) {
		uint8_t ch = bodyString[i];
		if (ch == '%') {
			if (i + 2 < bodyString.size()) {
				char hex1 = bodyString[i + 1];
				char hex2 = bodyString[i + 2];
				decoded << static_cast<char>(hexCharToInt(hex1) * 16 + hexCharToInt(hex2));
				i += 2;
			} else {
				LOG_ERROR("Body decoding error");
			}
		} else if (ch == '+') {
			decoded << ' ';
		} else if (ch == '&') {
			decoded << '\n';
		} else {
			decoded << ch;
		}
	}
	return decoded.str();
}

// std::string HttpResponse::urlDecode(const std::vector<uint8_t>& bodyString) {
// 	std::ostringstream decoded;
// 	for(size_t element = 0; element < bodyString.size(); element++){
// 		std::string bodyLine(bodyString[element].c_str());
// 		for (std::size_t i = 0; i < bodyLine.length(); ++i) {
// 			if (bodyString[i] == '%') {
// 				if (i + 2 < bodyString.length()) {
// 					char hex1 = bodyString[i + 1];
// 					char hex2 = bodyString[i + 2];
// 					decoded << static_cast<char>(hexCharToInt(hex1) * 16 + hexCharToInt(hex2));
// 					i += 2;
// 				} else {
// 					// Could throw an exception
// 					LOG_ERROR("Body decoding error");
// 				}
// 			} else if (bodyString[i] == '+') {
// 				decoded << ' ';
// 			} else {
// 				decoded << bodyString[i];
// 			}
// 		}
// 	}
// 	return decoded.str();
// }

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
	std::string cgiBin = "/cgi_bin/";
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

std::string HttpResponse::getFilePath(const std::string& scriptPath, std::string root) const {
    size_t lastSlash = scriptPath.find_last_of('/');
    if (lastSlash == std::string::npos) {
        return root + '/' + ".";
    }
    return  root + scriptPath.substr(0, lastSlash);
}

void HttpResponse::setCgiResponse(const int cgi_pipe_fd, pid_t cgi_pid, const bool is_cgi_response) {
    _cgi_pipe_fd = cgi_pipe_fd;
    _cgi_pid = cgi_pid;
    _is_cgi_response = is_cgi_response;
}