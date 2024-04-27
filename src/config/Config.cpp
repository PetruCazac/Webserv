#include "Config.hpp"

// //--------------------- Http Config---------------------//
// Directives for http block :
// keepalive_timeout		-- time to keep a connection alive with a client
// send_timeout 			-- timeout for CGI processes

// // ------------------- Server config -------------------//
// Directives for server block :
// autoindex 				-- List all the directories present in the accessed directory.
// client_max_body_size		-- Maximum size of the body sent by the client.
// error_page				-- List all the default files that would be returned in case of an error, if not only the error code will be shown.
// index					-- If a file is not found it will go to the file defined by index and will serve it.
// listen					-- The number of the port which tghe server will operate on.
// log_file					-- The name of the file where the logs will be stored.
// root						-- The root directory where the files will be searched.
// server_name				-- The name of the server.

// // ---------------- Location config ----------------------//
// Directives for server block :
// autoindex				-- Command to show the directory listing.
// error_page				-- Defines the default error pages  for each error that occurs. If there is no file, a simple error will be returned.
// fastcgi_param			-- Limit the FAST_CGI methods that are allowed to be used in this location.
// index					-- If the page does not exist, the page defined by index will be returned. If there is no page, the error will be returned.
// limit_except				-- Limit the HTTP methods that are allowed to be used in this location.
// module					-- The directory from the root where the files will be searched.
// root						-- redefines the root path for the location it is being used.


Config::Config(){}
Config::~Config(){}
Config::Config(const Config&){}
Config& Config::operator=(const Config&){
	return *this;
}

void Config::tokenize(const char* configFile){
	std::ifstream fs(configFile, std::ios_base::in);
	checkFilename(configFile);
	if(!fs.is_open()){
		LOG_ERROR("Failed to open config file.");
		throw OpenException();
	}
	std::string line;
	while(std::getline(fs, line)){
		std::size_t posComment = line.find('#');
		while(std::isspace(line[0])){
			line.erase(0, 1);
		}
		if(posComment != std::string::npos)
			line = line.substr(0, posComment);
		if(!line.empty() && line.find_last_of(";{}") != line.size() - 1)
			throw MissingLastCharacter();
		std::istringstream lineStream(line);
		std::string token;
		while(lineStream >> token){
			std::string specialChar("{};");
			size_t end = 0, start = 0;
			while((end = token.find_first_of(specialChar, start)) != std::string::npos){
				if(end > start)
					tokens.push_back(token.substr(start, end-start));
				tokens.push_back(token.substr(end, 1));
				start = end + 1;
			}
			if(start < token.size()){
				tokens.push_back(token.substr(start));
			}
		}
	}
	tokenIndex = 0;
}

void Config::parse(void){
	Block block;
	block.name = "root";
	while(tokenIndex < tokens.size()){
		block.children.push_back(parseDirective());
	}
	printDirective(block, 0);
	parseConfig(block);
	std::cout << "\n"<< std::endl;
	printConfig();
}

void Config::parseConfig(Block& block){
	if(block.name == "root"){
		for(size_t i = 0; i < block.children.size(); i++)
			parseConfig(block.children[i]);
	}else if(block.name == "http"){
		if(block.methods.size() != 0)
			throw WrongMethods();
		HttpDirectives http;
		getHttpStruct(http);
		http.name = block.name;
		parseHttp(http, block);
		_httpConfig = http;
		for(size_t i = 0; i < block.children.size(); i++){
			if(block.children[i].name == "location"){
				throw WrongLocationDeclaration();
			} else if(block.children[i].name == "server")
				parseConfig(block.children[i]);
		}
	} else if(block.name == "server"){
		if(block.methods.size() != 0)
			throw WrongMethods();
		ServerDirectives server;
		getServerStruct(server);
		server.name = block.name;
		parseServer(server, block);
		for(size_t i = 0; i < block.children.size(); i++){
			if(block.children[i].name == "location"){
				LocationDirectives location;
				getLocationStruct(location);
				parseLocation(location, block.children[i]);
				server.locations.push_back(location);
			
			} else if(block.children[i].name == "server")
				throw WrongServerDeclaration();
		}
		_serversConfig.push_back(server);
	}
}

Block Config::parseDirective(){
	Block block;
	block.name = tokens[tokenIndex++];
	while (tokenIndex < tokens.size() && tokens[tokenIndex] != "{") {
		block.methods.push_back(tokens[tokenIndex++]);
	}
	if (tokenIndex < tokens.size() && tokens[tokenIndex] == "{") {
		tokenIndex++;
		while (tokenIndex < tokens.size() && tokens[tokenIndex] != "}") {
			if(isBlock())
				block.children.push_back(parseDirective());
			else
				block.parameters.push_back(tokens[tokenIndex++]);
		}
		if (tokenIndex < tokens.size())
			tokenIndex++;
	} else if (tokenIndex < tokens.size() && tokens[tokenIndex] == ";") {
		tokenIndex++;
	}
	return block;
}

bool Config::isBlock(){
	size_t it = tokenIndex;
	while(it < tokens.size()){
		if(tokens[it][tokens[it].size() - 1] == '{')
			return true;
		else if(tokens[it][tokens[it].size() - 1] == ';')
			return false;
		it++;
	}
	throw MissingLastCharacter();
	return false;
}

void Config::checkFilename(const char* configFile){
	std::string conf(configFile);
	if(conf.size() < 5){
		LOG_ERROR("Invalid Configuration File Name");
		throw InvalidFilename();
		return;
	}
	if(conf.substr(conf.size() - 5, conf.size()) != ".conf"){
		LOG_ERROR("Invalid Configuration File Name");
		throw InvalidFilename();
		return;
	}
	return;
}

std::vector<ServerDirectives> Config::getServerConfig(void){
	std::vector<ServerDirectives> config = _serversConfig;
	return config;
}

// ---------------------------------------------------- Helper Functions ---------------------------------------- //

bool isNumber(std::string str){
	std::stringstream ss(str);
	double n;
	if(!(ss >> n))
		return false;
	return true;
}

void logInfo(std::string message, std::string directive, std::vector<std::string> attributes){
	std::stringstream ss;
	ss << message << ": ";
	ss << directive << " ";
	for(size_t i = 0; i < attributes.size(); i++)
		ss << attributes[i];
	LOG_INFO(ss.str());
}

void Config::parseHttp(HttpDirectives& http, Block& block){
	std::map<std::string, std::vector<std::string> > directives;
	for(size_t i = 0; i < block.parameters.size(); i++){
		size_t count = i + 1;
		while(count < block.parameters.size() && block.parameters[count] != ";")
			directives[block.parameters[i]].push_back(block.parameters[count++]);
		i = count;
	}
	for(std::map<std::string, std::vector<std::string> >::iterator it = directives.begin(); it != directives.end(); it++){
		if(it->first == "keepalive_timeout"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			std::stringstream ss;
			ss << it->second[0];
			ss >> http.keepalive_timeout;
		}else if(it->first == "send_timeout"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			std::stringstream ss;
			ss << it->second[0];
			ss >> http.send_timeout;
		} else
			logInfo("Unknown directive, will be ignored:", it->first, it->second);
	}
}

void Config::parseServer(ServerDirectives& server, Block& block){
	std::map<std::string, std::vector<std::string> > directives;
	for(size_t i = 0; i < block.parameters.size(); i++){
		size_t count = i + 1;
		while(count < block.parameters.size() && block.parameters[count] != ";")
			directives[block.parameters[i]].push_back(block.parameters[count++]);
		i = count;
	}
	// ------- Parse the directives ----------
	for(std::map<std::string, std::vector<std::string> >::iterator it = directives.begin(); it != directives.end(); it++){
		if(it->first == "server_name"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			server.server_name = it->second[0];
		}else if(it->first == "autoindex"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			server.autoindex = it->second[0];
		}else if(it->first == "index"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			server.index = it->second[0];
		}else if(it->first == "log_file"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			server.log_file = it->second[0];
		}else if(it->first == "root"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			server.root = it->second[0];
		}else if(it->first == "client_max_body_size"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			std::stringstream ss;
			ss << it->second[0];
			ss >> server.client_max_body_size;
		}else if(it->first == "listen"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			std::stringstream ss;
			ss << it->second[0];
			ss >> server.listen;
		} else
			logInfo("Unknown directive, will be ignored:", it->first, it->second);
	}
	// checkServerDirectives(server);
}

void Config::parseLocation(LocationDirectives& location, Block& block){
	if(block.methods.size() != 1 || block.methods[0].at(0) != '/')
		throw WrongMethods();
	else
		location.module = block.methods[0];
	// ------- Parse the directives ----------
	std::map<std::string, std::vector<std::string> > directives;
	for(size_t i = 0; i < block.parameters.size(); i++){
		size_t count = i + 1;
		while(count < block.parameters.size() && block.parameters[count] != ";")
			directives[block.parameters[i]].push_back(block.parameters[count++]);
		i = count;
	}
	for(std::map<std::string, std::vector<std::string> >::iterator it = directives.begin(); it != directives.end(); it++){
		if(it->first == "autoindex"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			location.autoindex = it->second[0];
		}else if(it->first == "index"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			location.index = it->second[0];
		}else if(it->first == "root"){
			if(it->second.size() != 1)
				throw Config::WrongDirectiveAttributes();
			location.root = it->second[0];
		}else if(it->first == "fastcgi_param"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			location.fastcgi_param.push_back(it->second[0]);
		}else if(it->first == "limit_except"){
			if(it->second.size() != 1 || !isNumber(it->second[0]) || it->second.size() > 12)
				throw Config::WrongDirectiveAttributes();
			location.limit_except.push_back(it->second[0]);
		} else
			logInfo("Unknown directive, will be ignored:", it->first, it->second);
	}
	if(block.methods.size() == 1 && block.methods[0].at(0) == '/')
		location.module = block.methods[0];
	else
		throw Config::WrongMethods();
	// checkLocationDirectives(server);
}

// ----------------- Get Default Structures -----------------

void Config::getHttpStruct(HttpDirectives& http){
	http.keepalive_timeout = DefaultValues::getDefaultValue<int>(KEEP_ALIVE_TIMEOUT);
	http.send_timeout = DefaultValues::getDefaultValue<int>(SEND_TIMEOUT);
}

void Config::getServerStruct(ServerDirectives& server){
	server.autoindex = DefaultValues::getDefaultValue<std::string>(AUTOINDEX);
	server.client_max_body_size = DefaultValues::getDefaultValue<double>(CLIENT_MAX_BODY_SIZE);
	server.index = DefaultValues::getDefaultValue<std::string>(INDEX);
	server.listen = DefaultValues::getDefaultValue<int>(LISTEN);
	server.log_file = DefaultValues::getDefaultValue<std::string>(LOG_FILE);
	server.root = DefaultValues::getDefaultValue<std::string>(ROOT);
}

void Config::getLocationStruct(LocationDirectives& location){
	location.autoindex = DefaultValues::getDefaultValue<std::string>(AUTOINDEX);
	location.index = DefaultValues::getDefaultValue<std::string>(INDEX);
	location.root = DefaultValues::getDefaultValue<std::string>(ROOT);
	location.fastcgi_param.push_back(DefaultValues::getDefaultValue<std::string>(FASTCGI_PARAM));
	location.limit_except.push_back(DefaultValues::getDefaultValue<std::string>(LIMIT_EXCEPT));
}

// ---------------------------------------------- Printing Functions -----------------------------------------//

void Config::printConfig(){
	std::cout << "//---------------------- HTTP STRUCTURE---------------------//" << std::endl;
	std::cout << "name: " << _httpConfig.name << std::endl;
	std::cout << "keepalive_timeout: " << _httpConfig.keepalive_timeout << std::endl;
	std::cout << "send_timeout: " << _httpConfig.send_timeout << std::endl;

	for(size_t i = 0; i < _serversConfig.size(); i++){
	std::cout << "//---------------------- SERVER STRUCTURE---------------------//" << std::endl;
		std::cout << "name: " << _serversConfig[i].name << std::endl;
		std::cout << "server_name: " << _serversConfig[i].server_name << std::endl;
		std::cout << "autoindex: " << _serversConfig[i].autoindex << std::endl;
		std::cout << "client_max_body_size: " << _serversConfig[i].client_max_body_size << std::endl;
		std::cout << "index: " << _serversConfig[i].index << std::endl;
		std::cout << "listen: " << _serversConfig[i].listen << std::endl;
		std::cout << "log_file: " << _serversConfig[i].log_file << std::endl;
		std::cout << "root: " << _serversConfig[i].root << std::endl;
		for(size_t j = 0; j < _serversConfig[i].locations.size(); j++){
			std::cout << "//---------------------- LOACATION STRUCTURE---------------------//" << std::endl;
			std::cout << "LOCATION module: " << _serversConfig[i].locations[j].module << std::endl;
			std::cout << "LOCATION autoindex: " << _serversConfig[i].locations[j].autoindex << std::endl;
			std::cout << "LOCATION fastcgio_params: " << _serversConfig[i].locations[j].fastcgi_param[0] << std::endl;
			std::cout << "LOCATION index: " << _serversConfig[i].locations[j].index << std::endl;
			std::cout << "LOCATION limit_except: " << _serversConfig[i].locations[j].limit_except[0] << std::endl;
			std::cout << "LOCATION root: " << _serversConfig[i].locations[j].root << std::endl;
		
		}
	}
}

void Config::printDirective(Block& block, int depth = 0){
	std::string indent(depth * 2, ' ');
	std::cout << indent << block.name << std::endl;
	if(!block.methods.empty()){
		std::cout << indent <<"Methods: ";
		for (size_t i = 0; i < block.methods.size(); ++i){
			std::cout << " " << block.methods[i];
		}
		std::cout << std::endl;
	}
	if(!block.parameters.empty()){
		std::cout <<  indent << "Directives: ";
		for (size_t i = 0; i < block.parameters.size(); ++i){
			std::cout << " " << block.parameters[i] << std::endl;
		}
		std::cout << std::endl;
	}
	if(!block.children.empty()){
		std::cout <<  indent << "Children: " << std::endl;
		for (size_t i = 0; i < block.children.size(); ++i){
			printDirective(block.children[i], depth + 2);
		}
	}
}