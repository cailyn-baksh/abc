#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <queue>

enum class Tokens {
	INC,
	DEC,
	NEXT,
	PREV,
	BEGINLOOP,
	ENDLOOP,
	OUT,
	IN
};

std::queue<Tokens> parse(std::string file);

#endif  // _PARSER_HPP_

