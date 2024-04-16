#include "Block.hpp"

Block::Block(){}

Block::Block(std::deque<std::string> directive){
	iteratorDeque it = directive.begin();
	getBlock(directive, it, *this);
}
Block::~Block(){}
Block::Block(const Block& ){}
Block& Block::operator=(const Block& ){
	return *this;
}



Block::iteratorDeque& Block::getBlock(stringDeque& directive, iteratorDeque& it, Block& b){




}
// 	while (it < directive.end()){
// 		if ((*it) == "{"){
// 			Block b;
// 			subBlock.push_back(b);
// 			it = getBlock(directive, ++it, b);
// 		} else if ((*it) == "}"){
// 			it++;
// 			return it;
// 		} else {
// 			b.directive.push_back((*it));
// 			it++;
// 		}
// 	}
// 	return it;
// }