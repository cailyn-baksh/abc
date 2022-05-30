#ifndef _IR_HPP_
#define _IR_HPP_

/*
 * The intermediate representation is a "virtual machine" designed for
 * compiling esoteric langauges. The IR is a cell-based virtual machine.
 *
 * Virtual Machine Description
 * ===========================
 *
 * Memory
 * The virtual machine treats memory as an infinite tape of cells.
 *
 * Registers
 * There are eight registers in the virtual machine. cp is the cell pointer. The
 * rest of the registers are general purpose.
 *  - cp
 *  - r0
 *  - r1
 *  - r2
 *  - r3
 *  - r4
 *  - r5
 *  - r6
 *
 * Flags
 * The special flags register contains bit flags that are set as a result of
 * certain instructions. The flags are Z (zero), N (negative), C (carry), and
 * V (overflow).
 *
 * Instruction Format
 * ==================
 *
 * The IR uses a bytecode to represent IR instructions during compilation.
 * The general format of the IR bytecode is a 1 byte opcode followed by any
 * arguments the opcode requires.
 *
 * The instruction format is generally defined as a four bit instruction,
 * followed by two bits for the first operand/destination and two for the
 * second operand.
 *
 * The following table shows opcodes and what operand types are allowed. r
 * indicates a register, m indicates a memory address, o indicates a
 * pc-relative offset, and i indicates an immediate value. c indicates a
 * condition code.
 *	0	jmp c,rmo
 *	1	add rmo,rmoi
 *	2	sub rmo,rmoi
 *	3	mul rmo,rmoi
 *	4	div rmo,rmoi
 *	5	cmp rmo,rmoi
 *	6	tst rmo,rmoi
 *	7	and rmo,rmoi
 *	8	or rmo,rmoi
 *	9	xor rmo,rmoi
 *	A	not rmo
 *	B	lsl rmo,rmoi
 *	C	lsr rmo,rmoi
 *	D	mov rmo,rmoi
 *	E	call rmo
 *	F	ret
 *
 * Operand Type Encoding
 * The remaining four bytes of the opcode specify the types of the
 * instruction's arguments. Each argument is specified by a two bit sequence.
 * For instructions with fewer than two arguments, the remaining bits are
 * ignored.
 * 	00	Register
 *	01	Memory Address
 *	10	PC-Relative Offset
 *	11	Immediate value
 * 
 * Register encoding
 * Registers are encoded as a three byte sequence.
 *	000		cp
 *	001		r0
 *	010		r1
 *	011		r2
 *	100		r3
 *	101		r4
 *	110		r5
 *	111		r6
 * Opcodes which specify two register arguments are to be packed into a single
 * byte, with the 2 least significant bits ignored.
 *
 * Memory address encoding
 *
 * PC-Relative Offset Encoding
 * PC-relative offsets are encoded in a single byte. The value of this byte is
 * signed, and is added to the program counter.
 *
 * Immediate encoding
 *
 * Condition Codes
 * Condition codes are represented by a four bit sequence. The most significant
 * bit is the negate bit, which determines whether the condition should be
 * negated at the end of evaluation. The negate bit is true when its value is
 * zero (so that null bytes evaluate to jmp !al,cp which is effectively a nop)
 * The remaining three bytes of the condition code are the actual condition
 *  Code	Condition	Mnemonic	!Condition	Mnemonic
 *	000		1			al			0			-
 *	001		Z			eq	z		!Z			ne	nz
 *	010		C			cs	hs		!C			cc	lo
 *	011		N			mi			!N			pl
 *	100		V			vs			!V			vc
 *	101		C & !Z		hi			!C | Z		ls
 *	110		N = V		ge			N != V		lt
 *	111		N = V & !Z	gt			N != V | Z	le
 */

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
	enum class Register {
		CP = 0b000,
		R0 = 0b001,
		R1 = 0b010,
		R2 = 0b011,
		R3 = 0b100,
		R4 = 0b101,
		R5 = 0b110,
		R6 = 0b111
	};

	enum class Condition {
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
	 * Represents an instruction.
	 */
	class Instruction {
	private:
		Opcode opcode;
		std::optional<std::variant<Register, std::uint8_t, Condition>> op1;
		std::optional<std::variant<Register, std::uint8_t>> op2;

	public:
		/*
		 * Creates a new instruction 
		 */
		Instruction(Opcode opcode);

		/*
		 * Adds a register argument to the instruction.
		 */
		Instruction &arg(Register r);

		/*
		 * Adds a PC-relative offset argument to the instruction.
		 */
		Instruction &arg(std::uint8_t o);

		Instruction &arg(Condition c);
	};
}

#endif  // _IR_HPP_

