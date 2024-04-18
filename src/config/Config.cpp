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
			if(token.empty())
				continue;
			line.push_back(token);
		}
		if (line.empty() || (*line.begin()).find_first_of("#", 0) == 0)
			continue;
		directives.push_back(line);
	}
	if(directives.size() == 0){
		throw EmptyConfFile();
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
		itDeque it = directives.begin();
		itDeque it_end = directives.end() - 1;
		parseBlock(it, it_end, directives);
	}catch(ParsingExceptions& e){
		std::cout << e.what() << std::endl;
	}
}

Config::itDeque	Config::parseBlock(itDeque& it, itDeque& it_end, strDeque& directives){
	itDeque	end_box;
	while(it < it_end){
		if(contextBlock((*it))){
			CONTEXT_TYPE = HTTP;
			end_box = endBlock(it, directives.end());
			it = parseBlock(++it, end_box, directives);
		} else if(serverBlock(*it)){
			end_box = endBlock(it, directives.end());
			it = getServerConfig(it, end_box);
			continue ;
		} else if (it < it_end){
			std::string str("Non valid directive found outside the block configuration: ");
			for(std::vector<std::string>::iterator itv = (*it).begin(); itv < (*it).end(); itv++)
				str.append((*itv));
			LOG_WARNING(str.c_str());
			throw WrongDirective();
		}
	}
	return it;
}

bool Config::contextBlock(std::vector<std::string>& line){

	CONTEXT_TYPE = HTTP;
	std::vector<std::string>::iterator it = line.begin();
	if((*it) == "http" && (*(it+1)) == "{" && line.size() == 2){
		std::cout << "Http Block\n";
		return true;
	}
	return false;
}

bool Config::serverBlock(std::vector<std::string>& line){

	CONTEXT_TYPE = UNDEF;
	std::vector<std::string>::iterator it = line.begin();
	if((*it) == "server" && (*(it+1)) == "{" && line.size() == 2){
		std::cout << "Server Block\n";
		return true;
	}
	return false;
}

Config::itDeque Config::getServerConfig(itDeque begin, itDeque end){
	itDeque it = begin;
	while(it < end){
		// Check last character
		std::vector<std::string> str = (*it);
		// std::string last = str.back();
		// if(last.back() == ';')
		// 	throw MissingLasCharacter();
		// Needs to not have the last character checked when first one is location

		// Check directive name
		std::cout << "HERE\n" ;
		if(str.front().c_str() == LOCATION){
			it = parseLocation(it, end);
		} else if (str.front().c_str() == LISTEN){
			parseDirective(LISTEN, str);
		} else if (str.front().c_str() == INDEX){
			parseDirective(INDEX, str);
		} else if (str.front().c_str() == LOCATION){
			parseDirective(LOCATION, str);
		} else if (str.front().c_str() == HOSTNAME){
			parseDirective(HOSTNAME, str);
		} else if (str.front().c_str() == SERVERNAME){
			parseDirective(SERVERNAME, str);
		} else if (str.front().c_str() == PORT){
			parseDirective(PORT, str);
		} else if (str.front().c_str() == ROOT){
			parseDirective(ROOT, str);
		} else {
			// throw WrongDirective();
		}


		// std::cout << "--------------------------" << std::endl;
		// for(std::vector<std::string>::const_iterator it = (*begin).begin(); it < (*begin).end(); it++){
		// 	std::cout << "[" << (*it) << "] ";
		// }
		// std::cout << "\n";
		begin++;
	}
	return ++end;
}


Config::itDeque Config::parseLocation(itDeque it, itDeque end){
	std::cout << (*it).front();
	end++;
	return it++;
}

void Config::parseDirective(const char* dir, std::vector<std::string> str){
	std::cout << dir << "\n";
	std::cout << str.front();
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
