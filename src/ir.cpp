#include "ir.hpp"

namespace IR {
	/* InvalidInstructionException */
	InvalidInstructionException::InvalidInstructionException(const char *msg) : message(msg) {}

	const char *InvalidInstructionException::what() {
		return message;
	}

	/* Operand */
	Operand::Operand() {}

	Operand::Operand(Register r) {
		this->type = REGISTER;
		this->value = r;
	}

	Operand::Operand(std::string s) {
		this->type = ADDR;
		this->value = s;
	}

	Operand::Operand(std::uint8_t o) {
		this->type = PC_RELATIVE;
		this->value = o;
	}

	Operand operator*(Register r) {
		Operand op;

		op.type = Operand::INDIRECT;
		op.value = r;

		return op;
	}

	/* Instruction */
	Instruction::Instruction(Opcode opcode, Operand op) : opcode(opcode) {
		if (opcode == CPL) {
			// op1
		} else if (opcode == CALL) {
			// op2
			op2 = op;
		} else {
			throw InvalidInstructionException("This opcode does not accept one operand");
		}
	}

	Instruction::Instruction(Opcode opcode, Operand op1, Operand op2) : opcode(opcode) {
		if (opcode != CPL || opcode != CALL || opcode != JMP) {
			this->op1 = op1;
			this->op2 = op2;
		} else {
			throw InvalidInstructionException("This opcode does not accept two operands");
		}
	}

	Instruction::Instruction(Opcode opcode, Condition cc, Operand op) : opcode(opcode) {
		if (opcode == JMP) {
			this->cc = cc;
			this->op2 = op;
		} else {
			throw InvalidInstructionException("This opcode does not accept a condition and an operand");
		}
	}

	/* Program */
	void Program::add_label(std::string label) {
		symTable[label] = instructions.size();
	}

	void Program::push_instructions(std::initializer_list<Instruction> instructions) {
		this->instructions.insert(this->instructions.end(), instructions);
	}
}
