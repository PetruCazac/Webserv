#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "Webserv.hpp"

class Block {
private:
	typedef std::deque<std::string> stringDeque;
	typedef std::deque<std::string>::iterator iteratorDeque;

public:
	std::deque<std::string>	directive;
	std::deque<Block>			subBlock;

	Block();
	Block(std::deque<std::string> directive);
	~Block();
	Block(const Block& b);
	Block& operator=(const Block& b);

	iteratorDeque& getBlock(stringDeque& directives, iteratorDeque& it, Block& b);

};

#endif