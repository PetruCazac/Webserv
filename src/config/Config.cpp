 #include "Config.hpp"
 #include "Block.hpp"

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

Config::Config(){}
Config::~Config(){}
Config::Config(const Config&){}
Config& Config::operator=(const Config&){
	return *this;
}

Config::Config(const char* configFile){
	std::ifstream fs(configFile, std::ios_base::in);
	checkFilename(configFile);
	if(!fs.is_open()){
		LOG_ERROR("Failed to open config file.");
		throw OpenException();
	}
	std::string str;
	std::deque<std::string> directives;
	while(std::getline(fs, str)){
		directives.push_back(str);
		str.clear();
	}
	if(!directives.size()){
		LOG_ERROR("Server config file is empty.");
	}
	getConfig(directives);
}

void Config::getConfig(std::deque<std::string> directives){
	try{
		checkBrackets(directives);
		Block block(directives);
	}catch(ParsingExceptions& e){
		std::cout << e.what() << std::endl;
	}
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

void Config::checkBrackets(const std::deque<std::string>& directives) const {
	int bracket = 0;
	int lineBracket = 0;
	for(std::deque<std::string>::const_iterator it\
		=directives.begin(); it < directives.end(); it++)
	{
		for(std::string::const_iterator it2 = (*it).begin(); it2 < (*it).end(); it2++)
		{
			if(*it2 == '{'){
				bracket++;
				lineBracket++;
			}
			else if(*it2 == '}'){
				bracket--;
				lineBracket++;
			}
			if(bracket < 0) {
				throw WrongBracket();
			}
		}
		if(lineBracket > 1)
			throw WrongBracket();
		lineBracket = 0;
	}
	if(bracket != 0)
		throw WrongBracket();
}