 #include "Config.hpp"

Config::Config(){}
Config::~Config(){}
Config::Config(const Config& c){}
Config& Config::operator=(const Config& c){}

Config::Config(const char* configFile){
	std::ifstream fs(configFile, std::ios_base::in);
	if(!fs.is_open()){
		LOG_ERROR("Failed to open config file.");
	}
	std::stringstream fileBuff;
	fileBuff << fs.rdbuf();
	std::string confString;
	while(fileBuff.eof())
		fileBuff >> confString;
	if(!confString.size()){
		LOG_ERROR("Server config file is empty.");
	}
	getServerConfig(confString);
}

void Config::getServerConfig(std::string& confString){
	std::string::iterator it = confString.begin();
	std::vector<std::string> server_block;

	while(it != confString.end()){

	}
}



