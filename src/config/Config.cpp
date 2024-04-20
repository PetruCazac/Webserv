 #include "Config.hpp"

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
	std::string line;
	while(std::getline(fs, line)){
		std::size_t posComment = line.find('#');
		if(posComment != std::string::npos)
			line = line.substr(0, posComment);
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
	int i = 0;
	printDirective(block, i);
	parseConfig(block);
}

void Config::parseConfig(Block& block){
	if(block.name == "http"|| block.name == "root"){
		for(size_t i = 0; i < block.children.size(); i++)
			parseConfig(block.children[i]);
	} else if(block.name == "server"){
		if(block.methods.size() != 0)
			throw WrongMethods();
		Server serv;
		assignDirective(block.parameters, serv);
		// assignMethods(block.methods, serv);
		// for(size_t i = 0; i < block.children.size(); i++)
		// 	assignChildren(block.children[i], serv);
		
	}
}

void Config::assignDirective(std::vector<std::string>& parameters, Server& s){
	for(size_t i = 0; i < parameters.size(); i++){
		if(parameters[i] == "index"){
			i += fillDirective(s._listen, parameters, ++i);
		} else if(parameters[i] == "server_name"){
			i += fillDirective(s._server_name, parameters, ++i);
		}
	}
}

size_t Config::fillDirective(std::vector<std::string>& s, std::vector<std::string>& parameters, size_t start){
	size_t count = start;
	while(count < parameters.size() && parameters[count] != ";"){
		s.push_back(parameters[count++]);
	}
	return ++count - start;
}

// void Config::assignDirective(std::vector<std::string>& parameters, server& s){
// 	for(size_t j = 0; j < parameters.size(); j++){
// 		for(size_t i = 0; i < TOTAL; i++){
// 			if(parameters[j] == Directives[i]){
// 				j++;
// 				while(parameters[j][parameters[j].size() - 1] != ';'){
					
// 				}
// 				break;
// 			}
// 		}
// 	}
// }

// void Config::assignMethods(std::vector<std::string>& methods, server& s){

// }

// void Config::assignChildren(Block& children, server& s){

// }

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

void Config::printDirective(Block& block, int depth = 0){
	std::string indent(depth * 2, ' ');
	std::cout << indent << block.name;
	std::cout << indent <<"Methods: ";
	for (size_t i = 0; i < block.methods.size(); ++i) {
		std::cout << " " << block.methods[i];
	}
	std::cout << std::endl;
	std::cout <<  indent << "Parametres: ";
	for (size_t i = 0; i < block.parameters.size(); ++i) {
		std::cout << " " << block.parameters[i];
	}
	std::cout << std::endl;
	std::cout <<  indent << "Children: ";
	for (size_t i = 0; i < block.children.size(); ++i) {
		printDirective(block.children[i], ++depth);
	}
}