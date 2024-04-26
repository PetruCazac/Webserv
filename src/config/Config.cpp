#include "Config.hpp"
#include "DefaultValues.hpp"

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

std::string translateDirectives(enum Parser directive){
	switch(directive) {
		case INDEX:
			return 	"index";
		case LISTEN:
			return "listen";
		case LOCATION:
			return "location";
		case SERVERNAME:
			return "server_name";
		case CLIENTSIZE:	
			return "client_max_body_size";
		case ROOT:
			return "root";
		case TRY_FILES:
			return "try_files";
		case LOG_FILE:
			return "log_file";
		case MAX_DATA_SIZE_INC:
			return "max_data_size_incoming";
		case LOG_LEVEL:
			return "log_level";
		case TOTAL:
			return NULL;
	}
	return NULL;
}

Parser getParseLevel(const std::string& str){
	if (str == "index") return INDEX;
	else if (str == "listen") return LISTEN;
	else if (str == "location") return LOCATION;
	else if (str == "host_name") return HOSTNAME;
	else if (str == "server_name") return SERVERNAME;
	else if (str == "client_max_body_size") return CLIENTSIZE;
	else if (str == "port") return PORT;
	else if (str == "root") return ROOT;
	else if (str == "try_files") return TRY_FILES;
	else if (str == "log_file") return LOG_FILE;
	else if (str == "max_data_size_incoming") return MAX_DATA_SIZE_INC;
	else if (str == "log_level") return LOG_LEVEL;
	else return TOTAL;
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
	// -----------------------------------------Parsing http part -------------------------------
	}else if(block.name == "http"){
		if(block.methods.size() != 0)
			throw WrongMethods();
		ServerDirectives server;
		server.name = block.name;
		for(size_t i = 0; i < block.parameters.size(); i++){
			if(isValidDirective(block.parameters[i])){
				size_t count = i + 1;
				while(count < block.parameters.size() && block.parameters[count] != ";")
					server._directives[block.parameters[i]].push_back(block.parameters[count++]);
				i = count;
			}
		}
		parseHttp(server._directives);
		_serversConfig.push_back(server);
		for(size_t i = 0; i < block.children.size(); i++){
			if(block.children[i].name == "location"){
				throw WrongLocationDeclaration();
			} else if(block.children[i].name == "server")
				parseConfig(block.children[i]);
		}
	// ------------------------------------------ Parsing server part ----------------------------
	} else if(block.name == "server"){
		if(block.methods.size() != 0)
			throw WrongMethods();
		ServerDirectives server;
		server.name = block.name;
		for(size_t i = 0; i < block.parameters.size(); i++){
			if(isValidDirective(block.parameters[i])){
				size_t count = i + 1;
				while(count < block.parameters.size() && block.parameters[count] != ";")
					server._directives[block.parameters[i]].push_back(block.parameters[count++]);
				i = count;
			}
		}
		for(size_t i = 0; i < block.children.size(); i++){
			if(block.children[i].name == "location"){
				server._location_methods = block.children[i].methods;
				for(size_t j = 0; j < block.children[i].parameters.size(); j++){
					if(isValidDirective(block.children[i].parameters[j])){
						size_t count = j + 1;
						while(count < block.children[i].parameters.size() && block.children[i].parameters[count] != ";")
							server._location_directives[block.children[i].parameters[j]].push_back(block.children[i].parameters[count++]);
						j = count;
					}
				}
			} else if(block.children[i].name == "server")
				parseConfig(block.children[i]);
		}
		_serversConfig.push_back(server);
	}
}

bool Config::isValidDirective(std::string str){
	if(str.size() == 0)
		throw MissingDirective();
	if (getParseLevel(str) == TOTAL)
		throw WrongDirective();
	return true;
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

std::vector<std::string>& getVector(std::string str){
	std::vector <std::string> v;
	v.push_back(str);
	return v;
}

void parseHttp(std::map<std::string, std::vector<std::string> >& directives){
	for(std::map<std::string, std::vector<std::string> >::iterator it = directives.begin(); it != directives.end(); it++){
		if(it->first == "client_max_body_size"){
			if(it->second.size() == 0){
				LOG_INFO("Missing directive will be assigned default value.");
				directives["client_max_body_size"] = getVector(DefaultValues::CLIENT_MAX_BODY_SIZE);
			}else if(it->second.size() > 1 || !isNumber(it->second[0]))
				throw Config::WrongDirectiveAttributes();
		}else if(it->first == "keepalive_timeout"){
			if(it->second.size() == 0){
				LOG_INFO("Missing directive will be assigned default value.");
				std::vector <std::string> v;
				v.push_back(DefaultValues::FILE_LOG);
				directives["client_max_body_size"] = v;
			}else if(it->second.size() != 1 || !isNumber(it->second[0]))
				throw Config::WrongDirectiveAttributes();
		}else if(it->first == "send_timeout"){
			if(it->second.size() != 1 || !isNumber(it->second[0]))
				throw Config::WrongDirectiveAttributes();
		} else
			logInfo("Unknown directive", it->first, it->second);
	}
}

// ---------------------------------------------- Printing Functions -----------------------------------------//

void Config::printConfig(){
	if(!_serversConfig.empty()){
		for (size_t i = _serversConfig.size() - 1; ; --i){
			const ServerDirectives& server = _serversConfig[i];
			std::cout << server.name << ":" << std::endl;
			// if(!server._directives.empty()){
				std::cout << "  Directives:" << std::endl;
				for (std::map<std::string, std::vector<std::string> >::const_iterator it = server._directives.begin(); it != server._directives.end(); ++it){
					std::cout << "    " << it->first << ": ";
					for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit){
						std::cout << *vit << " ";
					}
					std::cout << std::endl;
				}
			// }
			// if(!server._location_directives.empty()){
				std::cout << "  Location Directives:";
				for (std::map<std::string, std::vector<std::string> >::const_iterator lit = server._location_directives.begin(); lit != server._location_directives.end(); ++lit){
					std::cout << "    " << "##"<<  lit->first << "##"<< ": ";
					for (std::vector<std::string>::const_iterator lvit = lit->second.begin(); lvit != lit->second.end(); ++lvit){
						std::cout << "##"<<*lvit << "##"<< " ";
					}
					std::cout << std::endl;
				}
			// }
			// if(!server._location_methods.empty()){
			std::cout << "  Location Methods: ";
			for (std::vector<std::string>::const_iterator mit = server._location_methods.begin(); mit != server._location_methods.end(); ++mit){
				std::cout << "##"<< *mit <<"##";
			}
			std::cout << "\n" << std::endl;
			if(i == 0)
				break;
			// }
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
			std::cout << " " << block.parameters[i];
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