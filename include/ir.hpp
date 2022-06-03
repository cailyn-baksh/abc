#ifndef _IR_HPP_
#define _IR_HPP_

#include <exception>
#include <optional>
#include <variant>

#include <cstdint>

namespace IR {
	/*
	 * Represents an opcode
 	 */
	enum class Opcode {
		JMP  = 0x0,
		ADD  = 0x1,
		SUB  = 0x2,
		MUL  = 0x3,
		DIV  = 0x4,
		CMP  = 0x5,
		TST  = 0x6,
		AND  = 0x7,
		OR   = 0x8,
		XOR  = 0x9,
		NOT  = 0xA,
		LSL  = 0xB,
		LSR  = 0xC,
		MOV  = 0xD,
		CALL = 0xE,
		RET  = 0xF
	};

	/*
	 * Represents a register
	 */
	enum Register {
		CP = 0b000,
		R0 = 0b001,
		R1 = 0b010,
		R2 = 0b011,
		R3 = 0b100,
		R4 = 0b101,
		R5 = 0b110,
		R6 = 0b111
	};

	enum Condition {
		AL = 0b1000, NV = 0b0000,
		Z  = 0b1001, NZ = 0b0001,
		CS = 0b1010, CC = 0b0010,
		MI = 0b1011, PL = 0b0011,
		VS = 0b1100, VC = 0b0100,
		HI = 0b1101, LS = 0b0101,
		GE = 0b1110, LT = 0b0110,
		GT = 0b1111, LE = 0b0111
	};

	class InvalidInstructionException : public std::exception {
	private:
		const char *message;
	public:
		InvalidInstructionException(const char *msg);
		const char *what();
	};

	/*
	 * Represents an IR instruction. This class makes use of operator overloading
	 * to allow it to be used in a format mirroring assembly language.
	 *
	 * Some examples of use
	 *
	 * add r0,[r1]
	 * ADD (R0) [R1];
	 */
	class Instruction {
	public:
		/*
		 * Create a new instruction.
		 *
		 * opcode - The opcode of the new instruction.
		 */
		Instruction(Opcode opcode);

		/*
		 * Apply a register argument to an instruction
		 *
		 * r - The register
		 */
		Instruction &operator()(Register r);

		/*
		 * Apply a register indirect argument to an instruction
		 *
		 * r - The register
		 */
		Instruction &operator[](Register r);

		Instruction &operator()(Condition c);
	};

}

#define IR_JMP	Instruction(Opcode::JMP)
#define IR_ADD	Instruction(Opcode::ADD)
#define IR_SUB	Instruction(Opcode::SUB)
#define IR_MUL	Instruction(Opcode::MUL)
#define IR_DIV	Instruction(Opcode::DIV)
#define IR_CMP	Instruction(Opcode::CMP)
#define IR_TST	Instruction(Opcode::TST)
#define IR_AND	Instruction(Opcode::AND)
#define IR_OR	Instruction(Opcode::OR)
#define IR_XOR	Instruction(Opcode::XOR)
#define IR_NOT	Instruction(Opcode::NOT)
#define IR_LSL	Instruction(Opcode::LSL)
#define IR_LSR	Instruction(Opcode::LSR)
#define IR_MOV	Instruction(Opcode::MOV)
#define IR_CALL	Instruction(Opcode::CALL)
#define IR_RET	Instruction(Opcode::RET)

#endif  // _IR_HPP_

