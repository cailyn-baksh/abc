#include "ir.hpp"

using namespace IR;

InvalidInstructionException::InvalidInstructionException(const char *msg) : message(msg) {}

const char *InvalidInstructionException::what() {
	return message;
}

Instruction::Instruction(Opcode opcode) : opcode(opcode) {

}

Instruction &Instruction::arg(Condition c) {
	if (opcode != Opcode::JMP) {
		throw InvalidInstructionException("Condition code is only valid on jmp");
	} else if (op1) {
		throw InvalidInstructionException("First argument already present");
	}

	op1 = c;

	return *this;
}

Instruction &Instruction::arg(Register r) {
	if (opcode == Opcode::RET) {
		throw InvalidInstructionException("Register argument is not valid for this opcode");
	}

	if (!op1) {
		op1 = r;
	} else {
		op2 = r;
	}

	return *this;
}

Instruction &Instruction::arg(std::uint8_t o) {
	if (opcode == Opcode::RET) {
		throw InvalidInstructionException("PC-Relative offset argument is not valid for this opcode");
	}

	if (!op1) {
		op1 = o;
	} else {
		op2 = o;
	}

	return *this;
}

