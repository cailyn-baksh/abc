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
	/*
	 * Thrown when an IR instruction is configured incorrectly.
	 */
	class InvalidInstructionException : public std::exception {
	private:
		const char *message;
	public:
		/*
		 * Construct a new InvalidInstructionException with a message
		 *
		 * msg	A message explaining this exception.
		 */
		InvalidInstructionException(const char *msg);

		/*
		 * Returns the message of the exception.
		 */
		const char *what();
	};

	/*
	 * IR opcodes
	 */
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
	 * IR registers
	 */
	enum Register {
		R0 = 0b000,
		R1 = 0b001,
		R2 = 0b010,
		R3 = 0b011,
		R4 = 0b100,
		R5 = 0b101,
		R6 = 0b110,
		R7 = 0b111
	};
	// Alternate register names
	const Register AR = R6;
	const Register LR = R7;

	/*
	 * IR conditions. The NV condition is added to represent the never
	 * condition (referred to as !always in the docs).
	 */
	enum Condition {
		AL = 0b1000, NV = 0b0000,
		EQ = 0b1001, NE = 0b0001,
		CS = 0b1010, CC = 0b0010,
		MI = 0b1011, PL = 0b0011,
		VS = 0b1100, VC = 0b0100,
		HI = 0b1101, LS = 0b0101,
		GE = 0b1110, LT = 0b0110,
		GT = 0b1111, LE = 0b0111
	};
	// Alternate condition code names
	const Condition Z = EQ;
	const Condition NZ = NE;
	const Condition HS = CS;
	const Condition LO = CC;

	/*
	 * An instruction operand. This type differs from the actual encoding of
	 * the instruction operands. This struct can represent registers, register
	 * indirects, symbols, and literals. It is up to the code generator to
	 * decide how to actually encode these operand values in IR code.
	 */
	struct Operand {
		enum {
			REGISTER,
			INDIRECT,
			SYMBOL,
			LITERAL
		} type;
		std::variant<Register, std::string, std::uintmax_t> value;

		/*
		 * Construct a new operand. This constructor does nothing; if you use
		 * it you must initialize the struct values yourself.
		 */
		Operand();

		/*
		 * Construct a new register operand. This is not and cannot be a
		 * register indirect.
		 *
		 * reg	The register named in this operand.
		 */
		Operand(Register reg);

		/*
		 * Construct a new symbol operand. A symbol is a string which can
		 * either be a label, which is resolved by the compiler backend, or an
		 * external symbol which is resolved by the linker.
		 *
		 * sym	The name of the symbol
		 */
		Operand(std::string sym);

		/*
		 * An literal integer value.
		 *
		 * lit	The value
		 */
		Operand(std::uintmax_t lit);
	};

	/*
	 * Represents an instruction in the IR. This representation is an
	 * abstraction of the actual bytecode representation; it is meant to store
	 * the instructions' data until it is time to assemble the instructions
	 * into IR code to pass to the next phase in the pipeline.
	 */
	class Instruction {
		friend class _InstructionPtr;
		friend class Program;

	private:
		Opcode opcode;

		Condition cc = AL;
		std::optional<Operand> op1;
		std::optional<Operand> op2;

		bool useCC = false;
		bool useOp1 = false;
		bool useOp2 = false;

	public:
		/*
		 * Construct a new instruction with the given opcode
		 */
		Instruction(Opcode opcode);
	};

	/*
	 * A helper class for Instruction and Program. This class is not meant to
	 * be instantiated by the programmer. It is returned by members of Program
	 * as a pointer to Instructions within that Program instance, and provides
	 * functions and operators that can be used to manipulate that Instruction
	 * within the Program.
	 */
	class _InstructionPtr {
		friend class Program;

	private:
		Instruction *ptr;

		/*
		 * Construct a new _InstructionPtr that points to Instruction i
		 */
		_InstructionPtr(Instruction *i);

	public:
		/*
		 * Add a register argument to the instruction
		 *
		 * reg	The register argument to add
		 * Returns a reference to this
		 */
		_InstructionPtr &operator()(Register reg);

		/*
		 * Adds a register indirect argument to the instruction
		 *
		 * reg	The register indirect argument to add
		 * Returns a reference to this
		 */
		_InstructionPtr &operator[](Register reg);

		/*
		 * Add a symbol argument
		 *
		 * sym	The symbol argument to add
		 * Returns a reference to this
		 */
		_InstructionPtr &operator()(std::string sym);

		/*
		 * Add a integer literal argument
		 *
		 * lit	The literal argument to add
		 * Returns a reference to this
		 */
		_InstructionPtr &operator()(std::uintmax_t lit);

		_InstructionPtr &operator()(Condition cc);
	};

	/*
	 * An abstract representation of an IR program. This class provides an
	 * assembly-like interface to create IR bytecode.
	 */
	class Program {
	private:
		std::vector<Instruction *> instructions;

		/* The symbol table */
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

		/*
		 * Assemble this program into IR bytecode. After calling this function,
		 * the Program object is invalid and will not assemble properly again
		 *
		 * Returns the bytecode in a vector.
		 * Throws InvalidInstructionException if there is an error in the
		 * Instructions.
		 */
		std::vector<std::uint8_t> assemble();

		~Program();
	};
}

#endif  // _IR_HPP_

