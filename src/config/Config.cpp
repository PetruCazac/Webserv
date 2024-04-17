 #include "Config.hpp"

C_TYPES CONTEXT_TYPE;

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
	strDeque directives;

	while(std::getline(fs, str)){
		std::stringstream ss(str);
		std::string	token;
		std::vector<std::string> line;
		while(ss >> token){
			// std::cout << "[" << token << "]" << std::endl;
			if(token.empty())
				continue;
			line.push_back(token);
		}
		if (line.empty() || (*line.begin()).find_first_of("#", 0) == 0 )
			continue;
		directives.push_back(line);
	}

	if(!directives.size()){
		LOG_ERROR("Server config file is empty.");
	}
	getConfig(directives);
}

bool Config::lineComment(const std::string word) const {
	if(word.find_first_of("#", 0) == 0)
		return true;
	return false;
}

void Config::getConfig(strDeque directives){
	try{
		checkBrackets(directives);
		// for(citDeque it\
		// = directives.begin(); it < directives.end(); it++){
		// 	std::cout << "--------------------------" << std::endl;
		// 	for(std::vector<std::string>::const_iterator it2 = (*it).begin(); it2 < (*it).end(); it2++){
		// 		std::cout << "[" << (*it2) << "]" << std::endl;
		// 	}
		// 	std::cout << "--------------------------" << std::endl;
		// }
		itDeque it = directives.begin();
		parseBlock(it, directives);
	}catch(ParsingExceptions& e){
		std::cout << e.what() << std::endl;
	}
}


void	Config::parseBlock(itDeque it, strDeque directives){
	while(it < directives.end()){
		if(contextBlock((*it))){
			CONTEXT_TYPE = HTTP;
			itDeque it_end = endBlock(it, directives.end());
			while(it <= it_end){
				it = getServerConfig(it, it_end);
				it++;
			}
		} else if(serverBlock(*it)){
			CONTEXT_TYPE = UNDEF;
			itDeque it_end = endBlock(it, directives.end());
			it = getServerConfig(it, it_end);
			it++;
		}
		
	}
}


bool Config::serverBlock(std::vector<std::string> line){
	std::vector<std::string>::iterator it = line.begin();
	if((*it) == "server" && (*(it+1)) == "{" && line.size() == 2)
		return true;
	return false;
}
bool Config::contextBlock(std::vector<std::string> line){
	std::vector<std::string>::iterator it = line.begin();
	if((*it) == "http" && (*(it+1)) == "{" && line.size() == 2)
		return true;
	return false;
}

Config::itDeque Config::getServerConfig(itDeque begin, itDeque end) const{
	// while(begin < end)
	begin ++ ;
	end ++;
	std::cout<< "Server" << std::endl;
	return --end;
}

Config::itDeque Config::endBlock(itDeque it, itDeque end){
	int i = 0;
	while(it < end){
		for(std::vector<std::string>::iterator sit = (*it).begin(); sit < (*it).end(); sit++){
			if((*sit) == "{")
				i++;
			else if((*sit) == "}")
				i--;
		}
		if(i == 0)
			return it;
		it++;
	}
	throw WrongBracket();
	return it;
}

void Config::checkBrackets(const strDeque& directives) const {
	int bracket = 0;
	int lineBracket = 0;
	for(citDeque it\
		=directives.begin(); it < directives.end(); it++)
	{
		for(std::vector<std::string>::const_iterator it2 = (*it).begin(); it2 < (*it).end(); it2++)
		{
			if(*it2 == "{"){
				bracket++;
				lineBracket++;
			} else if(*it2 == "}"){
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

// bool Config::openBracket(const std::string str){
// 	int i = 0;
// 	if((i = str.find('{')) > -1){
// 		int j = str.size();
// 		while(std::isspace(str[j]))
// 			j--;
// 		if(i == str[j])
// 			return true;
// 	}
// 	return false;
// }

// bool Config::closedBracket(const std::string str){
// 	int i = 0;
// 	if((i = str.find('}')) > -1){
// 		int j = str.size();
// 		while(std::isspace(str[j]))
// 			j--;
// 		if(i == str[j])
// 			return true;
// 	}
// 	return false;
// }