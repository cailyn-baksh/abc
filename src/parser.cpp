#include <fstream>
#include <string>
#include <queue>

#include "parser.hpp"

std::queue<Tokens> parse(std::string file) {
	std::ifstream in(file);

	if (!in) {
		// Failed to open file
		//return;
	}

	std::queue<Tokens> tokens;

	char c;
	while (in.get(c)) {
		switch (c) {
			case '+':
				tokens.push(Tokens::INC);
				break;
			case '-':
				tokens.push(Tokens::DEC);
				break;
			case '>':
				tokens.push(Tokens::NEXT);
				break;
			case '<':
				tokens.push(Tokens::PREV);
				break;
			case '[':
				tokens.push(Tokens::BEGINLOOP);
				break;
			case ']':
				tokens.push(Tokens::ENDLOOP);
				break;
			case '.':
				tokens.push(Tokens::OUT);
				break;
			case ',':
				tokens.push(Tokens::IN);
		}
	}

	return tokens;
}
