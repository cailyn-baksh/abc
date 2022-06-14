#include <array>
#include <queue>
#include <utility>
#include <variant>

#include <cstddef>
#include <cstdint>

#include "ir.hpp"

namespace IR {
	/*******************************
	 * InvalidInstructionException *
	 *******************************/
	InvalidInstructionException::InvalidInstructionException(const char *msg) : message(msg) {}

	const char *InvalidInstructionException::what() {
		return message;
	}


	/***********
	 * Program *
	 ***********/
	void Program::label(std::string lbl) {
		symTable[lbl] = instructions.size();
	}

	_InstructionPtr Program::operator()(Opcode opcode) {
		Instruction *instr = new Instruction(opcode);

		instructions.push_back(instr);

		return _InstructionPtr(instr);
	}

	_InstructionPtr Program::operator()(Pseudoinstruction pseudo) {
		Instruction *instr = nullptr;

		switch (pseudo) {
			case NOP:
				// jump with null address and a 'never' condition
				instr = new Instruction(JMP);
				instr->cc = NV;
				instr->op2 = Operand(0);
				instr->size = BYTE;
				break;
			case RET:
				// jmp lr
				instr = new Instruction(JMP);
				instr->op2 = Operand(LR);
				break;
		}

		return _InstructionPtr(instr);
	}

	std::vector<std::uint8_t> Program::assemble() {
		std::vector<std::uint8_t> prog;

		// during the first assembly pass, all symbols are skipped over, with
		// null bytes written in their place. the symbol fixup queue is a queue
		// of all the locations in prog where this was done, with the name of
		// the local symbol which should be used
		std::queue<std::pair<std::size_t, std::string>> symbolFixupQueue;
		// associates locate symbol names with their positions in prog
		std::map<std::string, std::uint32_t> localSymbols;

		for (Instruction *instruction : instructions) {
			// Start by checking if a local label points here
			for (auto &[key, value] : symTable) {
				if (instructions[value] == instruction) {  // pointer comparison not object comparison
					// this symbol points to this instruction; record this in the fixup queue
					localSymbols[key] = prog.size();
					symTable.erase(key);  // remove from symtable to make future loops faster
										  // if we dont do this the object will be reusable...
					break;
				}
			}

			std::uint8_t instructionByte = 0;

			// Populate instruction byte
			instructionByte |= static_cast<std::uint8_t>(instruction->opcode) << 4;

			if (instruction->op1) {
				// has op1
				switch (instruction->op1->type) {
					case Operand::REGISTER:
						// bit 3 is already 0
						break;
					case Operand::INDIRECT:
						instructionByte |= (1 << 3);
						break;
					default:
						// Invalid
						throw InvalidInstructionException("Not a valid operand type for op1");
				}
			}

			if (instruction->op2) {
				// has op2
				switch (instruction->op2->type) {
					case Operand::REGISTER:
						// bit[2:1] are already 0
						break;
					case Operand::INDIRECT:
						instructionByte |= (0b01 << 1);
						break;
					case Operand::SYMBOL:
						instructionByte |= (0b10 << 1);
						break;
					case Operand::LITERAL:
						instructionByte |= (0b11 << 1);
						break;
				}
			}

			prog.push_back(instructionByte);

			// Encode the rest
			std::uint8_t scratch = 0;
			int scratch_pos = 8;

			OperandSize opSize = WORD;

			/*
			 * Makes enough room in scratch to fit n bits
			 */
			auto scratch_make_room = [&prog, &scratch, &scratch_pos](std::size_t n) {
				if (scratch_pos - n < 0) {
					// Not enough room
					prog.push_back(scratch);
					scratch = 0;
					scratch_pos = 8;
				} else {
					scratch_pos -= n;
				}
			};

			/*
			 * Flush scratch into program
			 */
			auto scratch_flush = [&prog, &scratch, &scratch_pos]() {
				if (scratch_pos != 8) {
					prog.push_back(scratch);
					scratch = 0;
					scratch_pos = 8;
				}
			};

			if (instruction->opcode == JMP) {
				// Special case; need to encode condition code first
				// cc always immediately follows JMP so no need to worry about
				// values in scratch
				scratch_pos -= 4;
				if (instruction->cc) {
					scratch |= static_cast<std::uint8_t>(*instruction->cc) << scratch_pos;
				} else {
					// if no cc was specified then default to AL
					scratch |= AL << scratch_pos;
				}
			}

			if (instruction->useOpSize) {
				// Operand size specifier
				// operand size is not valid with jump instruction, so no room
				// checking necessary
				scratch_pos -= 2;
				if (instruction->size) {
					opSize = *instruction->size;
				}

				scratch |= static_cast<std::uint8_t>(opSize) << scratch_pos;
			}

			// Now we add the rest of the operands

			if (instruction->op1) {
				// Op1. This can only be a register/register indirect so it is
				// always 3 bits.
				// No type checking needed since that was already done when
				// encoding the instruction byte.
				// No need to make room since at this point there always will
				// be at least 4 bits left
				scratch_pos -= 3;
				scratch |= (static_cast<std::uint8_t>(std::get<Register>(instruction->op1->value)) << scratch_pos);
			}

			if (instruction->op2) {
				// Op2

				switch (instruction->op2->type) {
					case Operand::REGISTER:
					case Operand::INDIRECT:
						scratch_make_room(3);  // Make room for 3 bits
						scratch |= (static_cast<std::uint8_t>(std::get<Register>(instruction->op2->value)) << scratch_pos);
						break;
					case Operand::SYMBOL:
						// TODO: figure out how to determine which byte in prog a label refers to
						{
							scratch_flush();

							std::string symbol = std::get<std::string>(instruction->op2->value);
							if (symTable.contains(symbol)) {
								// Local label; will fixup later
								symbolFixupQueue.push(std::make_pair(prog.size(), symbol));
								prog.insert(prog.end(), {0, 0, 0, 0});
							} else {
								// External symbol; put the name for the linker
								for (const char &ch : symbol) {
									prog.push_back(ch);
								}
							}
							break;
						}
					case Operand::LITERAL:
						// we can encode the value immediately rather than using a literal pool
						scratch_flush();

						// treat the address of the literal as the beginning of
						// an array, so that we can access each byte
						// individually
						std::uintmax_t value = std::get<std::uintmax_t>(instruction->op2->value);
						std::uint8_t *bytes = (uint8_t *)&value;

						for (std::size_t i=0; i < (1U << static_cast<unsigned int>(opSize)); ++i) {
							prog.push_back(bytes[i]);
						}

						break;
				}
			}


			if (scratch_pos != 8) {
				prog.push_back(scratch);
			}
		}

		// fixup local symbols
		while (!symbolFixupQueue.empty()) {
			auto fixup = symbolFixupQueue.front();
			std::uint8_t *bytes = (std::uint8_t *)&localSymbols[fixup.second];

			for (std::size_t i=0; i < sizeof(std::uint32_t); ++i) {
				prog[fixup.first + i] = bytes[i];
			}

			symbolFixupQueue.pop();
		}

		return prog;
	}

	Program::~Program() {
		for (Instruction *instruction : instructions) {
			delete instruction;
		}
	}


	/*******************
	 * _InstructionPtr *
	 *******************/
	_InstructionPtr::_InstructionPtr(Instruction *i) : ptr(i) {}

	_InstructionPtr &_InstructionPtr::operator()(OperandSize size) {
		if (ptr->useOpSize && !ptr->size) {
			ptr->size = size;
		} else {
			throw InvalidInstructionException("Cannot use operand size specifier here");
		}

		return *this;
	}

	// Add register
	_InstructionPtr &_InstructionPtr::operator()(Register reg) {
		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = reg;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = reg;
		} else {
			throw InvalidInstructionException("Cannot use register parameter here");
		}

		return *this;
	}

	// Add [register]
	_InstructionPtr &_InstructionPtr::operator[](Register reg) {
		Operand operand;
		operand.type = Operand::INDIRECT;
		operand.value = reg;

		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = operand;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = operand;
		} else {
			throw InvalidInstructionException("Cannot use register indirect parameter here");
		}

		return *this;
	}

	// Add symbol
	_InstructionPtr &_InstructionPtr::operator()(std::string sym) {
		if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = sym;
		} else {
			throw InvalidInstructionException("Cannot use symbol parameter here");
		}

		return *this;
	}

	// Add literal
	_InstructionPtr &_InstructionPtr::operator()(std::uintmax_t lit) {
		if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = lit;
		} else {
			throw InvalidInstructionException("Cannot use PC-relative offset parameter here");
		}

		return *this;
	}

	_InstructionPtr &_InstructionPtr::operator()(Condition cc) {
		if (ptr->useCC && !ptr->cc) {
			ptr->cc = cc;
		} else {
			throw InvalidInstructionException("Cannot use condition code here");
		}

		return *this;
	}


	/***************
	 * Instruction *
	 ***************/
	Instruction::Instruction(Opcode opcode) : opcode(opcode) {
		if (opcode == JMP) {
			useCC = true;
			useOp2 = true;
		} else if (opcode == CPL) {
			useOpSize = true;
			useOp1 = true;
		} else if (opcode == CALL) {
			useOp2 = true;
		} else {
			useOpSize = true;
			useOp1 = true;
			useOp2 = true;
		}
	}


	/***********
	 * Operand *
	 ***********/
	Operand::Operand() {}

	Operand::Operand(Register reg) : type(REGISTER), value(reg) {}

	Operand::Operand(std::string sym) : type(SYMBOL), value(sym) {}

	Operand::Operand(std::uintmax_t lit) : type(LITERAL), value(lit) {}
}

