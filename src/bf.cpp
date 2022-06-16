/*
 * Brainfuck frontend implementation
 */

#include <fstream>
#include <stack>

#include <iostream>

#include "frontend.hpp"
#include "ir.hpp"

void BrainfuckFrontend::applyOptions(char option, std::vector<std::string> &values) {

}

std::vector<std::uint8_t> BrainfuckFrontend::parse(std::string &file) {
	std::ifstream in(file);
	IR::Program program;

	std::stack<std::string> loopStack;  // loop label prefixes

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
				// add byte [ar],1
				program(IR::ADD) (IR::BYTE) [IR::AR](1);
				break;
			case '-':
				// sub byte [ar],1
				program(IR::SUB) (IR::BYTE) [IR::AR](1);
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

				// _start:
				//   tst byte [ar],[ar]
				//   jmp z,_end
				program.label(label + "_start");
				program(IR::TST) (IR::BYTE) [IR::AR][IR::AR];
				program(IR::JMP) (IR::Z)(label + "_end");
				break;
			case ']':
				label = loopStack.top();
				loopStack.pop();

				// _end:
				//   tst byte [ar],[ar]
				//   jmp nz,_start
				program.label(label + "_end");
				program(IR::TST) (IR::BYTE) [IR::AR][IR::AR];
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

	std::cout << program;

	return program.assemble();
}


std::string BrainfuckFrontend::helpStr() {
	return "";
}

void BrainfuckFrontend::setVerbosity(bool verbosity) {

}
