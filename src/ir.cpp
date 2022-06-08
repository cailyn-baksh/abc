#include <array>
#include <variant>

#include "ir.hpp"

namespace IR {
	/* InvalidInstructionException */
	InvalidInstructionException::InvalidInstructionException(const char *msg) : message(msg) {}

	const char *InvalidInstructionException::what() {
		return message;
	}


	/* Program */
	void Program::label(std::string lbl) {
		symTable[lbl] = instructions.size();
	}

	_InstructionPtr Program::operator()(Opcode opcode) {
		Instruction *instr = new Instruction(opcode);

		instructions.push_back(instr);

		return _InstructionPtr(instr);
	}

	std::vector<std::uint8_t> Program::assemble() {
		std::vector<std::uint8_t> prog;
		std::uint8_t *literalPool = new std::uint8_t[32];
		// consider taking frequently used values from the literal pool and loading them into registers
		// (is this done by the CPU cache already?)

		for (Instruction *instruction : instructions) {
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
			int scratch_pos = 7;

			/*
			 * Make enough room in scratch to fit n bits
			 */
			auto scratch_make_room = [&prog, &scratch, &scratch_pos](std::size_t n) {
				if (scratch_pos - n < 0) {
					// Not enough room
					prog.push_back(scratch);
					scratch = 0;
					scratch_pos = 7;
				} else {
					scratch_pos -= n;
				}
			};

			if (instruction->opcode == JMP) {
				// Special case; need to encode condition code first
				// cc always immediately follows JMP so no need to worry about
				// values in scratch
				scratch_pos -= 4;
				scratch |= (static_cast<std::uint8_t>(instruction->cc) << scratch_pos);
			}

			// Now we add the rest of the operands

			if (instruction->op1) {
				// Op1. This can only be a register/register indirect so it is
				// always 3 bits.
				// No type checking needed since that was already done when
				// encoding the instruction byte.
				// No need to make room since at this point there always will be 4 bits left
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
							std::string symbol = std::get<std::string>(instruction->op2->value);
							if (symTable.contains(symbol)) {
								// Local label
								if (scratch_pos != 7) {
									prog.push_back(scratch);
									scratch_pos = 7;
								}

								prog.insert(prog.end(), {0, 0, 0, 0});
							} else {
								// External symbol
							}
							break;
						}
					case Operand::LITERAL:
						// put the literal in the literal pool
						break;
				}

			}
		}

		delete literalPool;
		return prog;
	}

	Program::~Program() {
		for (Instruction *instruction : instructions) {
			delete instruction;
		}
	}


	/* _InstructionPtr */
	_InstructionPtr::_InstructionPtr(Instruction *i) : ptr(i) {}

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
		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = sym;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = sym;
		} else {
			throw InvalidInstructionException("Cannot use symbol parameter here");
		}

		return *this;
	}

	// Add literal
	_InstructionPtr &_InstructionPtr::operator()(std::uintmax_t lit) {
		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = lit;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = lit;
		} else {
			throw InvalidInstructionException("Cannot use PC-relative offset parameter here");
		}

		return *this;
	}


	/* Instruction */
	Instruction::Instruction(Opcode opcode) : opcode(opcode) {
		if (opcode == JMP) {
			useCC = true;
			useOp2 = true;
		} else if (opcode == CPL) {
			useOp1 = true;
		} else if (opcode == CALL) {
			useOp2 = true;
		} else {
			useOp1 = true;
			useOp2 = true;
		}
	}


	/* Operand */
	Operand::Operand() {}

	Operand::Operand(Register reg) {
		this->type = REGISTER;
		this->value = reg;
	}

	Operand::Operand(std::string sym) {
		this->type = SYMBOL;
		this->value = sym;
	}

	Operand::Operand(std::uintmax_t lit) {
		this->type = LITERAL;
		this->value = lit;
	}
}

