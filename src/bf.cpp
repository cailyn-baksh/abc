/*
 * Brainfuck frontend implementation
 */

#include <fstream>
#include <stack>

#include "frontend.hpp"
#include "ir.hpp"

void BrainfuckFrontend::applyOptions(char option, std::vector<std::string> &values) {

}

std::vector<std::uint8_t> BrainfuckFrontend::parse(std::string &file) {
	std::ifstream in(file);
	IR::Program program;

	std::stack<std::string> loopStack;

	// Create a unique label for loops
	// basically just returns a string with any ASCII character between
	// '!' (0x21) and '~' (0x7E)
	auto getUniqueLoopLabel = [](){
		static std::string prevLbl = " ";

		if (prevLbl.back() >= '~') {
			prevLbl += "!";
		} else {
			prevLbl.back()++;
		}

		return prevLbl;
	};

	if (!in) {
		// Failed to open file
	}

	// Set up program
	program.label("main");

	char c;
	while (in.get(c)) {
		std::string label;

		switch (c) {
			case '+':
				// add [ar],1
				program(IR::ADD) [IR::AR](1);
				break;
			case '-':
				// sub [ar],1
				program(IR::SUB) [IR::AR](1);
				break;
			case '>':
				// add ar,1
				program(IR::ADD) (IR::AR)(1);
				break;
			case '<':
				// sub ar,1
				program(IR::SUB) (IR::AR)(1);
				break;
			case '[':
				label = getUniqueLoopLabel();
				loopStack.push(label);

				program.label(label + "_start");
				program(IR::TST) [IR::AR][IR::AR];
				program(IR::JMP) (IR::Z)(label + "_end");
				break;
			case ']':
				label = loopStack.top();
				loopStack.pop();

				program.label(label + "_end");
				program(IR::TST) [IR::AR][IR::AR];
				program(IR::JMP) (IR::NZ)(label + "_start");
				break;
			case '.':
				// call putc
				program(IR::CALL) ("putc");
				break;
			case ',':
				// call getc
				program(IR::CALL) ("getc");
				break;
		}
	}

	return program.assemble();
}


std::string BrainfuckFrontend::helpStr() {
	return "";
}

void BrainfuckFrontend::setVerbosity(bool verbosity) {

}
