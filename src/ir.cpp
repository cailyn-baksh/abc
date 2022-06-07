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

	Program::~Program() {
		for (Instruction *instruction : instructions) {
			delete instruction;
		}
	}


	/* _InstructionPtr */
	_InstructionPtr::_InstructionPtr(Instruction *i) : ptr(i) {}

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

	_InstructionPtr &_InstructionPtr::operator[](Register reg) {
		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = reg;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = reg;
		} else {
			throw InvalidInstructionException("Cannot use register indirect parameter here");
		}

		return *this;
	}

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

	_InstructionPtr &_InstructionPtr::operator()(std::uint8_t off) {
		if (ptr->useOp1 && !ptr->op1) {
			ptr->op1 = off;
		} else if (ptr->useOp2 && !ptr->op2) {
			ptr->op2 = off;
		} else {
			throw InvalidInstructionException("Cannot use PC-relative offset parameter here");
		}
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
}

