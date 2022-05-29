#ifndef _CODEGEN_HPP_
#define _CODEGEN_HPP_

#include <queue>

#include "parser.hpp"

class AbstractCodeGenerator {
protected:
	bool optimizeConsecutive;
	bool unrollLoops;

public:
	AbstractCodeGenerator();

	void generate(std::queue<tokens>);
};

#endif  // _CODEGEN_HPP_

