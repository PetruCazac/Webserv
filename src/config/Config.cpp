 #include "Config.hpp"

Config::Config(){}
Config::~Config(){}
Config::Config(const Config& c){}
Config& Config::operator=(const Config& c){}

// The constructor of the class will get the file name and it will
// open it and will extract the information from there.

// Parsing part: It will extract line per line and will have a counter that will
// count the parenthesis and nested parenthesis this way it will get the config block

// It will then go in each block and parse each directive that is supported
// and that is not a comment.

// Depending on the directive, it will parse it differently.

// If the directive is not supported it will output an error and stop the program from running

// The directives will be stored in the _servers vector. It will be then outputed
// to the default struct that will assign the default values to the servers if some
// information is missing. Howeve if crucial information is missing this will output 
// an error or a warning message(tbd).
// Each directive that will be default will be logged as an info log.


Config::Config(const char* configFile){
	std::ifstream fs(configFile, std::ios_base::in);
	if(!fs.is_open()){
		LOG_ERROR("Failed to open config file.");
	}
	std::stringstream fileBuff;
	std::string confString;
	fileBuff << fs.rdbuf();
	fileBuff >> confString;
	getBlocks(confString);
	// getServerConfig(confString);
}

void Config::getServerConfig(std::string& confString){
	// std::deque<std::string> directives;
	// while(fileBuff.eof()){
	// 	fileBuff >> temp;
	// 	directives.push_back(temp);
	// 	temp.clear();
	// }
	// if(!directives.size()){
	// 	LOG_ERROR("Server config file is empty.");
	// }
	// std::deque<std::string>::iterator it = directives.begin();
	// while(it <= directives.end()){
	// 	it = getBlock(directives, it);
	// }
		
// 	}
}

std::deque<std::string>::iterator& Config::getBlocks(std::string& confString)
{
	int bracketLevel = 0;
	std::deque<std::string>::iterator it = directives.begin();
	while(it != directives.end()){
		if(it)
	}

}