#ifndef _IR_HPP_
#define _IR_HPP_

#include <exception>
#include <initializer_list>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace IR {
	/*
	 * Represents an opcode
 	 */
	enum Opcode {
		JMP	= 0x0,	ADD  = 0x1,
		SUB	= 0x2,	MUL  = 0x3,
		DIV	= 0x4,	CMP  = 0x5,
		TST	= 0x6,	AND  = 0x7,
		OR	= 0x8,	XOR  = 0x9,
		CPL	= 0xA,	LSL  = 0xB,
		LSR	= 0xC,	ASR  = 0xD,
		MOV	= 0xE,	CALL = 0xF
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
	
	struct Operand {
		enum {
			REGISTER	= 0b00,
			ADDR		= 0b01,
			INDIRECT	= 0b10,
			PC_RELATIVE	= 0b11
		} type;
		std::variant<Register, std::string, std::uint8_t> value;

		Operand();
		Operand(Register r);
		Operand(std::string s);
		Operand(std::uint8_t o);
	};

	/*
	 * Creates a register-indirect operand from a register
	 */
	Operand operator*(Register r);

	/*
	 * Represents an IR Instruction
	 */
	class Instruction {
	private:
		Opcode opcode;
		std::optional<Condition> cc;
		std::optional<Operand> op1;
		std::optional<Operand> op2;

		bool allowCC = false;
		bool allowOp1 = false;
		bool allowOp2 = false;

	public:
		/*
		 * Construct a new instruction with one operand
		 *
		 * opcode	The opcode of the instruction
		 * op		The operand
		 */
		Instruction(Opcode opcode, Operand op);

		/*
		 * Construct a new instruction with two operands
		 *
		 * opcode	The opcode of the instruction
		 * op1		The first operand
		 * op2		The second operand
		 */
		Instruction(Opcode opcode, Operand op1, Operand op2);

		/*
		 * Construct a new instruction with a condition code and an operand
		 *
		 * opcode	The opcode of the instruction
		 * cc		The condition code
		 * op		The operand
		 */
		Instruction(Opcode opcode, Condition cc, Operand op);
	};

	/*
	 * Represents an IR program
	 */
	class Program {
	private:
		std::vector<Instruction> instructions;

		std::map<std::string, std::size_t> symTable;

	public:
		/*
		 * Adds a label to the symbol table. The label will point to the next data
		 * added to the program.
		 *
		 * label	The name of the label to add.
		 */
		void add_label(std::string label);

		/*
		 * Appends an instruction to the program.
		 *
		 * instruction	The instruction to add.
		 */
		void push_instructions(std::initializer_list<Instruction> instructions);
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

