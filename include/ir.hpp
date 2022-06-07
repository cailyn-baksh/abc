#ifndef _IR_HPP_
#define _IR_HPP_

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <cstdint>

/*
 * Using the IR
 *
 * The IR works similarly to an assembly language. An instance of Program is
 * created, and labels and instructions are added to the program.
 *
 * A program like
 *
 * main:
 *   mov r0,[r1]
 *   add r0,1
 *   mov [r1],r0
 *
 * Would be implemented like this
 *
 * Program prog;
 * prog.label("main");
 * prog (MOV) (R0)[R1];
 * prog (ADD) (R0)(1);
 * prog (MOV) [R1](R0);
 */

namespace IR {
	class InvalidInstructionException : public std::exception {
	private:
		const char *message;
	public:
		InvalidInstructionException(const char *msg);
		const char *what();
	};

	enum Opcode {
		JMP	= 0x0,	ADD = 0x1,
		SUB = 0x2,	MUL = 0x3,
		DIV = 0x4,	CMP = 0x5,
		TST = 0x6,	AND = 0x7,
		OR	= 0x8,	XOR = 0x9,
		CPL = 0xA,	LSL = 0xB,
		LSR = 0xC,	ASR = 0xD,
		MOV = 0xE, CALL = 0xF
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

	class Instruction {
		friend class _InstructionPtr;

	private:
		Opcode opcode;

		std::optional<Condition> cc;
		std::optional<Operand> op1;
		std::optional<Operand> op2;

		bool useCC = false;
		bool useOp1 = false;
		bool useOp2 = false;

	public:
		Instruction(Opcode opcode);
	};

	class _InstructionPtr {
		friend class Program;

	private:
		Instruction *ptr;

		_InstructionPtr(Instruction *i);

	public:
		/*
		 * Add a register argument to the instruction
		 */
		_InstructionPtr &operator()(Register reg);

		/*
		 * Adds a register indirect argument to the instruction
		 */
		_InstructionPtr &operator[](Register reg);

		/*
		 * Add a symbol argument
		 */
		_InstructionPtr &operator()(std::string sym);

		/*
		 * Add a pc-relative offset argument
		 */
		_InstructionPtr &operator()(std::uint8_t off);
	};

	class Program {
	private:
		std::vector<Instruction *> instructions;
		std::map<std::string, std::size_t> symTable;

	public:
		/*
		 * Add a label to the program
		 */
		void label(std::string lbl);

		/*
		 * Add an instruction with the given opcode to the program. The return value
		 * of this operator is only valid until the next call.
		 */
		_InstructionPtr operator()(Opcode opcode);

		~Program();
	};
}

#endif  // _IR_HPP_

