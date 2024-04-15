 #include "Config.hpp"

Config::Config(){}
Config::~Config(){}
Config::Config(const Config&){}
Config& Config::operator=(const Config&){
	return *this;
}

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
	if(!checkFilename(configFile)){
		LOG_ERROR("Invalid Configuration File Name");
		throw InvalidFilename();
	}
	if(!fs.is_open()){
		LOG_ERROR("Failed to open config file.");
		throw OpenException();
	}
	stringDeque directives;
	std::stringstream fileBuff;
	std::string temp;
	fileBuff << fs.rdbuf();
	while(fileBuff >> temp){
		directives.push_back(temp);
		temp.clear();
	}
	if(!directives.size()){
		LOG_ERROR("Server config file is empty.");
	}
	getConfig(directives);
}

void Config::getConfig(stringDeque& directives){
	iteratorDeque it = directives.begin();
	while (it < directives.end()){
		std::cout << "##" << (*it) << "##" << std::endl;
		it++;
	}
	try{
		getBlocks(directives, it);
	}catch(ParsingExceptions e){
		std::cout << e.what() << std::endl;
	}
}

bool Config::checkFilename(const char* configFile){
	std::string conf(configFile);
	if(conf.size() < 5)
		return false;
	
	if(conf.substr(conf.size() - 5, conf.size()) != ".conf")
		return false;
	return true;
}

Config::iteratorDeque& Config::getBlocks(stringDeque& directives, iteratorDeque& it)
{
	while (it < directives.end()){
		if (openBracket(it)){
			it = getBlocks(directives, it);
		} else if (closedBracket(it)){
			it++;
			return it;
		} else {
			it++;
		}
	}
	return it;
}


// bool Config::openBracket(const iteratorDeque& it){
	
// }

// bool Config::closedBracket(const iteratorDeque& it){

// }