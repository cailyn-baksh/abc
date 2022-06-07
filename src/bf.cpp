/*
 * Brainfuck frontend implementation
 */

#include <fstream>

#include "frontend.hpp"
#include "ir.hpp"

IR::Program BrainfuckFrontend::parse(std::string &file) {
	std::ifstream in(file);
	IR::Program program;

	if (!in) {
		// Failed to open file
	}

	// Set up program
	program.label("main");
	program (IR::MOV) (IR::R1)(1);

	char c;
	while (in.get(c)) {
		/*switch (c) {
			case '+':
				// add r0,r1
				program.push_instructions({IR::Instruction(IR::ADD, IR::R0, IR::R1)});
				break;
			case '-':
				// sub r0,r1
				program.push_instructions({IR::Instruction(IR::SUB, IR::R0, IR::R1)});
				break;
			case '>':
				// mov [cp],r0
				// add cp,r1
				// mov r0,[cp]
				program.push_instructions({
					IR::Instruction(IR::MOV, *IR::CP, IR::R0),
					IR::Instruction(IR::ADD, IR::CP, IR::R1),
					IR::Instruction(IR::MOV, IR::R0, *IR::CP)
				});
				break;
			case '<':
				// mov [cp],r0
				// sub cp,r1
				// mov r0,[cp]
				break;
			case '[':
				break;
			case ']':
				break;
			case '.':
				// call putc
				break;
			case ',':
				// call getc
				break;
		}*/
	}
}
