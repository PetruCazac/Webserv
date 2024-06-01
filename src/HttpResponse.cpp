
#include <map>
#include <string>
#include <istream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "UtilsHttp.hpp"
#include "Config.hpp"

HttpResponse::HttpResponse(const int code) {
	makeDefaultResponse(code);
}

HttpResponse::HttpResponse(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	if (request.getHttpVersion() != "HTTP/1.1")
		makeDefaultResponse(505);
	else {
		if (request.getMethod() == GET) {
			runGetMethod(config, request);
		}
		else if (request.getMethod() == POST)
			runPostMethod(config, request);
		else if (request.getMethod() == DELETE)
			runDeleteMethod(config, request);
		else
			makeDefaultResponse(501);
	}
}

void HttpResponse::makeDefaultResponse(const int code) {
	std::string errorBody = getErrorBody(code);
	_response << "HTTP/1.1 " << code << ' ' << StatusCodeMap::getInstance().getStatusCodeDescription(code)
			  << "\r\n" << "Content-Type: text/html" << "\r\n"
			  << "Content-Length: " << errorBody.size() << "\r\n\r\n"
			  << errorBody;
}

std::string HttpResponse::getErrorBody(const int code) {
		std::string responseImage;
		std::string responseMessage;
	if(code == 201  || code == 200){
		responseImage = "        <img src=\"/default_ressources/58b.jpeg\" alt=\"Error Image\">";
		responseMessage = "        <p>All good!</p>";
	}else {
		responseImage = "        <img src=\"/default_ressources/bonk-doge.gif\" alt=\"Error GIF\">";
		responseMessage = "        <p>Something went wrong. Please try again later.</p>";
	}
		
	std::stringstream body;
	body << "<!DOCTYPE html>"
		 << "<html lang=\"en\">"
		 << "<head>"
		 << "    <meta charset=\"UTF-8\">"
		 << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
		 << "    <title>Error Page</title>"
		 << "    <style>"
		 << "        body {"
		 << "            display: flex;"
		 << "            justify-content: center;"
		 << "            align-items: center;"
		 << "            height: 100vh;"
		 << "            margin: 0;"
		 << "            font-family: Arial, sans-serif;"
		 << "            background-color: #f0f0f0;"
		 << "        }"
		 << "        .container {"
		 << "            text-align: center;"
		 << "        }"
		 << "        .container img {"
		 << "            max-width: 100%;"
		 << "            height: auto;"
		 << "        }"
		 << "        h1 {"
		 << "            margin: 20px 0 10px;"
		 << "            font-size: 2.5em;"
		 << "            color: #333;"
		 << "        }"
		 << "        p {"
		 << "            margin: 0;"
		 << "            font-size: 1.2em;"
		 << "            color: #666;"
		 << "        }"
		 << "    </style>"
		 << "</head>"
		 << "<body>"
		 << "    <div class=\"container\">"
		 << responseImage
		 << "        <h1>" << code << " " << StatusCodeMap::getInstance().getStatusCodeDescription(code) << "</h1>"
		 << responseMessage
		 << "    </div>"
		 << "</body>"
		 << "</html>";
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

// -------------------------------- DELETE Method -------------------------------------

void HttpResponse::composeDeleteUrl(const ServerDirectives& server, const HttpRequest& request, std::string& path){
	path = request.getUri();
	if(path.empty() || path[0] != '/'){
		path.clear();
		return;
	}
	cleanPath(path, server.root);
	size_t i = 0;
	while ((i = path.find("%20", i)) != std::string::npos) {
		path.replace(i, 3, " ");
		i += 1;
	}
	if(!server.locations.empty()){
		if(path.find(server.locations[0].module, 0) == 0){
			path = server.locations[0].root + path;
			if(isFile(path.c_str())){
				LOG_INFO("File will be deleted at the path" + path);
				return;
			} else
				makeDefaultResponse(404);
		} else
			makeDefaultResponse(405);
	} else {
		if(path.find(server.post_dir, 0) == 0){
			path = server.root + path;
			if(isFile(path.c_str())){
				LOG_INFO("File will be deleted at the path" + path);
				return;
			} else
				makeDefaultResponse(404);
		} else
			makeDefaultResponse(405);
	}
	path.clear();
	return;
}

void HttpResponse::runDeleteMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request) {
	ServerDirectives server;
	chooseServerConfig(config, request, server);
	// Not supporting Delete Method
	if(isCGI(request.getUri())){
		makeDefaultResponse(405);
		return;
	}
	std::string path;
	if(!isMethodAllowed(server, "DELETE")){
		LOG_INFO("DELETE Method not allowed.");
		makeDefaultResponse(405);
		return;
	}
	composeDeleteUrl(server, request, path);
	if(path.empty()){
		LOG_INFO("Delete encountered a problem.");
		return;
	}
	if(std::remove(path.c_str()) == 0){
		LOG_INFO("FILE Deleted successfuly.");
		makeDefaultResponse(200);
	}else{
		LOG_INFO("FILE could not be deleted.");
		makeDefaultResponse(500);
	}
}

// --------------------------------- POST Method --------------------------------------

bool HttpResponse::storeFormData(const std::string& formData, std::string& path) {
	std::stringstream time_int;
	time_int << time(NULL);
	if(isDirectory(path.c_str())){
		std::string filename = path + "/" + "form_submission_" + time_int.str() + ".txt";
		std::ofstream outFile(filename.c_str(), std::ios_base::app);
		if (!outFile) {
			LOG_ERROR("Error opening file for writing: " + filename);
			return false;
		}
		outFile << formData;
		outFile.close();
		makeDefaultResponse(201);
		return true;
	} else {
		std::ofstream file(path.c_str(), std::ios_base::app);
		if(file.is_open()){
			file << formData;
			file.close();
			makeDefaultResponse(201);
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
	if(path.empty() || path[0] != '/'){
		path.clear();
		return;
	}
	size_t i = 0;
	while ((i = path.find("%20", i)) != std::string::npos) {
		path.replace(i, 3, " ");
		i += 1;
	}
	cleanPath(path, server.root);
	if(!server.locations.empty()){
		if(path.find(server.locations[0].module, 0) == 0){
			path = server.locations[0].root + path;
			if(isDirectory(path.c_str())){
				LOG_INFO("POST Request will be saved at: " + path);
				return;
			}
		}
	} else {
		if(path.find(server.post_dir, 0) == 0){
			path = server.root + path;
			if(isDirectory(path.c_str())){
				LOG_INFO("POST Request will be saved at: " + path);
				return;
			}
		}
	}
	size_t start = path.find_last_of('/', path.size());
	if(start != std::string::npos){
		std::string lastElement = path.substr(start + 1, path.size() - start - 1);
		if(MimeTypeDetector::getInstance().getMimeType(lastElement) != "application/octet-stream"){
			if(isDirectory(path.substr(0, start).c_str())){
					LOG_INFO("POST Request will be saved at a new file: " + path);
					return;
			} else
				LOG_ERROR("The requested path is not valid: " + path);
		}
	}
	path.clear();
	return;
}

std::string HttpResponse::createFilename(std::string& path, std::map<std::string, std::string>& metadata){
	std::string fileName = metadata.find("filename")->second;
	if(MimeTypeDetector::getInstance().getMimeType(fileName) != _contentType){
		fileName += MimeTypeDetector::getInstance().getExtension(_contentType);
		path = path + '/' + fileName;
	} else {
		path = path + '/' + fileName;
	}
	return path;
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
		makeDefaultResponse(415);
		return false;
	}
	if(metadata.find("name")->second == "file"){
		std::string information = package.substr(endMetadata + 4, package.size() - endMetadata - 4);
		if(information.empty() || metadata["Content-Type:"] == "application/octet-stream")
			return true;
		std::string filename = createFilename(path, metadata);
		if(filename.empty()){
			makeDefaultResponse(400);
			return false;
		}
		if(isFile(filename.c_str())){
			LOG_INFO("File already exists, overwriting not allowed at the: " + filename);
			makeDefaultResponse(200);
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
	makeDefaultResponse(201);
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
	makeDefaultResponse(200);
}

void HttpResponse::runPostMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	ServerDirectives server;
	chooseServerConfig(config, request, server);
	if(isCGI(request.getUri())){
		handleCGI(server, request);
		return;
	}
	std::string path;
	if(!isMethodAllowed(server, "POST"))
		makeDefaultResponse(405);
	
	composePostUrl(server, request, path);
	if(path.empty()){
		makeDefaultResponse(400);
		return;
	}
	std::string contentType(request.getHeaders().find("Content-Type")->second);
	if(contentType.empty()){
		LOG_ERROR("No Content-Type found.");
		makeDefaultResponse(400);
		return;
	}
	if(contentType == "multipart/form-data")
		handleMultipart(request, path);
	else if(contentType == "application/x-www-form-urlencoded"){
		if(!storeFormData(urlDecode(request.getBody()), path))
			makeDefaultResponse(400);
	}else
		makeDefaultResponse(400);
}

// --------------------------------- GET Method --------------------------------------

void HttpResponse::runGetMethod(const std::vector<ServerDirectives> &config, const HttpRequest &request){
	ServerDirectives server;
	chooseServerConfig(config, request, server);
	if(isCGI(request.getUri())){
		handleCGI(server, request);
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
		} else if(isIndex(path, server) && isFile(path.c_str())){
			readFile(path);
			setResponse();
		} else{
			makeDefaultResponse(404);
		}
	}else
		makeDefaultResponse(405);
}


bool HttpResponse::isIndex(std::string &path, ServerDirectives& config) {
	if(!config.locations.empty()){
		if(!config.locations[0].index.empty()){
			path = config.locations[0].root + '/' + config.locations[0].index;
			return true;
		}
	} else if(!config.index.empty()){
		path = config.root + '/' + config.index;
		return true;
	}
	return false;
}

void HttpResponse::readFile(std::string &path) {
	std::ifstream file(path.c_str());
	if (!file.is_open())
		makeDefaultResponse(500);
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
	size_t i = 0;
	while ((i = path.find("%20", i)) != std::string::npos) {
		path.replace(i, 3, " ");
		i += 1;
	}
	cleanPath(path, server.root);
	if(!server.locations.empty()){
		if(!server.locations[0].root.empty()){
			size_t pos;
			pos = path.find(server.locations[0].module);
			pos += server.locations[0].module.size();
			if(pos != std::string::npos)
				path = server.locations[0].root + path.substr(pos, path.size());
		} else{
			path = server.root + path;
		}
		return;
	}else{
		path = server.root + path;
		if(!isValidPath(path.c_str())){
			if(server.index.empty()){
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

std::string HttpResponse::parseArguments(const HttpRequest& request){
	if(request.getMethod() == POST){
		std::vector<uint8_t> body = request.getBody();
		std::stringstream ss;
		for(size_t i = 0; i < body.size(); i++)
			ss << static_cast<char>(body[i]);
		std::string arg = ss.str();
		return arg;
	} else
		return std::string();
}

std::string getPath(const char *path){
	std::string str(path);
	size_t start = str.find("/", 0);
	std::string withoutRoot;
	if(start != std::string::npos)
		withoutRoot = std::string(str, start, str.size());
	else
		withoutRoot = str;
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

bool HttpResponse::isValidPath(const char* path){
	struct stat fileInfo;
	if (stat(path, &fileInfo) == -1) {
		return false;
	}
	return S_ISREG(fileInfo.st_mode) || S_ISDIR(fileInfo.st_mode);
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
	if(uri.find(cgiBin) != 0)
		return false;
	if(!findCgiType(const_cast<std::string&>(uri)).empty())
		return true;
	return false;
}

void HttpResponse::handleCGI(const ServerDirectives &config, const HttpRequest &request){
	std::string scriptPath = request.getUri();
	std::string arg = parseArguments(request);


	std::string checkType = findCgiType(scriptPath);
	if (checkType.size() == 0){
		makeDefaultResponse(500);
		return;
	}
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		makeDefaultResponse(500);
		return;
	}
	pid_t pid = fork();
	if (pid == -1) {
		makeDefaultResponse(500);
		return;
	} else if (pid == 0) {
		std::vector<const char*> env;
		std::string requestMethod("REQUEST_METHOD=" + request.getMethodStr());
		env.push_back(requestMethod.c_str());
		std::string scriptName("SCRIPT_NAME=" + scriptPath);
		env.push_back(scriptName.c_str());
		std::string pathInfo("PATH_INFO=" + scriptPath);
		env.push_back(pathInfo.c_str());

		std::string queryString = request.getQuery();
		if (!queryString.empty()) {
			queryString = "QUERY_STRING=" + queryString;
			env.push_back(queryString.c_str());
		}

		std::string contentLength = request.getContentLength();
		if (!contentLength.empty()) {
			contentLength = "CONTENT_LENGTH=" + contentLength;
			env.push_back(contentLength.c_str());
		}
		env.push_back(NULL);

		std::string fullScriptPath = "." + scriptPath;
		std::string compiler = findCgiType(scriptPath);
		std::vector <const char*> argv;
		argv.push_back(compiler.c_str());
		argv.push_back(fullScriptPath.c_str());
		argv.push_back(NULL);

		if(request.getMethod() == POST)
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (chdir(getRoot(config).c_str()) == -1){
			LOG_ERROR("CHDIR_CALL_FAILED");
			exit(1);
		}
		if (execve(argv[0], const_cast<char**>(argv.data()), const_cast<char**>(env.data())) == -1 ) {
			LOG_ERROR("EXECVE failed");
			perror("execve");
            exit(1);
		}
		exit(0);
	} else {
		if(request.getMethod() == POST) {
			int result = write(pipefd[1], arg.c_str(), arg.size());
            if (result == -1) {
                LOG_ERROR("CGI for POST failed");
            	makeDefaultResponse(500);
		        return;
            }
            if (result == 0)
                LOG_WARNING("Write call returned 0.");
        }
		close(pipefd[1]); 
		fcntl(pipefd[0], F_SETFL, O_NONBLOCK); 
		_cgi_pipe_fd = pipefd[0];
		_cgi_pid = pid;
		_is_cgi_response = true;
	}
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

std::string HttpResponse::findCgiType(std::string& uri){
	size_t extensionPos = uri.find_last_of('.', uri.size());
	std::string extension = uri.substr(extensionPos, uri.size() - extensionPos);
	if(extension == ".py"){
		return std::string("/usr/bin/python3");
	} else if(extension == ".sh"){
		return std::string("/bin/bash");
	} else if(extension == ".php"){
		return std::string("/usr/bin/php");
	}
	return std::string("");
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
