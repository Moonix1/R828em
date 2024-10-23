#include <iostream>
#include <vector>
#include <assert.h>
#include <functional>
#include <thread>
#include <span>
#include <fstream>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

#define MEMORY_CAPACITY 1024 * 64
struct Memory {
	u8 data[MEMORY_CAPACITY];
};

struct CPU {
public:
	CPU(Memory mem)
		: memory(mem) {}

	void reset() {
		pc = 0xD000;
		sp = 0xB000;
		fbs = 0x8000;

		r0 = 0x0000;
		r1 = 0x0000;
		r2 = 0x0000;
		r3 = 0x0000;

		ra = 0x0000;
		ri = 0x0000;

		b0 = 0x00;
		b1 = 0x00;
		b2 = 0x00;
		b3 = 0x00;

		equal		= 0;
		zero		= 0;
		decimal		= 0;
		sign		= 0;
		carry		= 0;
		overflow	= 0;
		interrupt	= 0;
		breakf		= 0;

		std::fill(std::begin(memory.data), std::end(memory.data), 0x00);
		for (size_t i = 0; i < (64 * 1024) * (64 * 1024) * 3 ; ++i) {
            memory.data[fbs + i] = 0xFF;
        }
	}

	void load_addr(u16 addr, u8 byte_value) {
		memory.data[addr] = byte_value;
	}

	void execute(size_t &cycles) {
		if (cycles > 0) {
			u8 inst = fetch_byte(cycles);

			switch (inst) {
			case LR0: {
				u16 value = fetch_word(cycles);
				r0 = value;
			} break;
			case LR1: {
				u16 value = fetch_word(cycles);
				r1 = value;
			} break;
			case LR2: {
				u16 value = fetch_word(cycles);
				r2 = value;
			} break;
			case LR3: {
				u16 value = fetch_word(cycles);
				r3 = value;
			} break;
			case LB0: {
				u16 value = fetch_word(cycles);
				b0 = value;
			} break;
			case LB1: {
				u16 value = fetch_word(cycles);
				b1 = value;
			} break;
			case LB2: {
				u16 value = fetch_word(cycles);
				b2 = value;
			} break;
			case LB3: {
				u16 value = fetch_word(cycles);
				b3 = value;
			} break;
			case LDA: {
				u16 value = fetch_word(cycles);
				ra = value;
			} break;
			case LDI: {
				u16 value = fetch_word(cycles);
				ri = value;
			} break;
			case PUSH: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case BYTE: {
					u8 value = fetch_byte(cycles);
					memory.data[sp] = value;
					sp++;
				} break;
				case WORD: {
					u8 low_byte = fetch_byte(cycles);
					u8 high_byte = fetch_byte(cycles);
					memory.data[sp] = low_byte;
					memory.data[sp + 1] = high_byte;
					sp += 2;
				} break;
				default: {
					std::cerr << "Invalid value mode: "
						<< "0x" << std::hex << std::uppercase << static_cast<int>(mode) << std::endl;
					exit(1);
				} break;
				}

				cycles--;
			} break;
			case POP: {
				u8 mode = fetch_byte(cycles);

				switch (mode) {
				case BYTE: {
					u8 *dest = fetch_register_u8(cycles);
					*dest = memory.data[sp - 1];
					memory.data[sp - 1] = 0x00;
					sp--;
				} break;
				case WORD: {
					u16 *dest = fetch_register_u16(cycles);
					*dest = ((u16)memory.data[sp - 2] << 8) | (u16)memory.data[sp - 1];
					memory.data[sp - 2] = 0x00;
					memory.data[sp - 1] = 0x00;
					sp -= 2;
				} break;
				default: {
					std::cerr << "Invalid value mode: "
						<< "0x" << std::hex << std::uppercase << static_cast<int>(mode) << std::endl;
					exit(1);
				} break;
				}
			} break;
			case STB: {
				u16 *memory_addr = fetch_register_u16(cycles);
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 value = fetch_byte(cycles);
					memory.data[*memory_addr] = value;
				} break;
				case 0xA1: {
					u8 *value = fetch_register_u8(cycles);
					memory.data[*memory_addr] = *value;
				} break;
				}

				cycles--;
			} break;
			case STW: {
				u16 *memory_addr = fetch_register_u16(cycles);
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 low_byte = fetch_byte(cycles);
					u8 high_byte = fetch_byte(cycles);
					memory.data[*memory_addr] = low_byte;
					memory.data[*memory_addr + 1] = high_byte;
				} break;
				case 0xA1: {
					u16 *value = fetch_register_u16(cycles);
					memory.data[*memory_addr] = *value & 0xFF;
					memory.data[*memory_addr + 1] = (*value >> 8) & 0xFF;
				} break;
				}

				cycles--;
			} break;
			case LDB: {
				u8 *dest = fetch_register_u8(cycles);
				u16 *addr = fetch_register_u16(cycles);

				*dest = memory.data[*addr];
			} break;
			case LDW: {
				u16 *dest = fetch_register_u16(cycles);
				u16 *addr = fetch_register_u16(cycles);

				*dest = ((u16)memory.data[*addr] << 8) | (u16)memory.data[*addr + 1];
			} break;
			case ADD: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                *dest = *reg1 + *reg2;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = (*reg1 > 0xFFFF - *reg2);
                overflow = (((*reg1 ^ *reg2) & 0x8000) == 0) && (((*reg1 ^ *dest) & 0x8000) != 0);
            } break;
            case ADC: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                u16 carry_in = carry ? 1 : 0;

                *dest = *reg1 + *reg2 + carry_in;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = (*dest < *reg1 || *dest < *reg2 || carry_in);
                overflow = (((*reg1 ^ *reg2) & 0x8000) == 0) && (((*reg1 ^ *dest) & 0x8000) != 0);
            } break;
            case SUB: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                *dest = *reg1 - *reg2;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = (*reg1 < *reg2);
                overflow = (((*reg1 ^ *reg2) & 0x8000) != 0) && (((*reg1 ^ *dest) & 0x8000) != 0);
            } break;
            case SBB: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                u16 carry_in = carry ? 1 : 0;

                *dest = *reg1 - *reg2 - carry_in;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = (*reg1 < *reg2 + carry_in);
                overflow = (((*reg1 ^ *reg2) & 0x8000) != 0) && (((*reg1 ^ *dest) & 0x8000) != 0);
            } break;
            case MUL: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                *dest = *reg1 * *reg2;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = false;
                overflow = carry;
            } break;
            case DIV: {
                u16 *dest = fetch_register_u16(cycles);
                u16 *reg1 = fetch_register_u16(cycles);
                u16 *reg2 = fetch_register_u16(cycles);
                *dest = *reg1 / *reg2;

                zero = (*dest == 0);
                sign = (static_cast<i16>(*dest) < 0);
                carry = false;
                overflow = false;
            } break;
			case ADDB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                *dest = *reg1 + *reg2;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = (*reg1 > 0xFF - *reg2);
                overflow = (((*reg1 ^ *reg2) & 0x80) == 0) && (((*reg1 ^ *dest) & 0x80) != 0);
            } break;
            case ADCB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                u8 carry_in = carry ? 1 : 0;

                *dest = *reg1 + *reg2 + carry_in;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = (*dest < *reg1 || *dest < *reg2 || carry_in);
                overflow = (((*reg1 ^ *reg2) & 0x80) == 0) && (((*reg1 ^ *dest) & 0x80) != 0);
            } break;
            case SUBB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                *dest = *reg1 - *reg2;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = (*reg1 < *reg2);
                overflow = (((*reg1 ^ *reg2) & 0x80) != 0) && (((*reg1 ^ *dest) & 0x80) != 0);
            } break;
            case SBBB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                u8 carry_in = carry ? 1 : 0;

                *dest = *reg1 - *reg2 - carry_in;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = (*reg1 < *reg2 + carry_in);
                overflow = (((*reg1 ^ *reg2) & 0x80) != 0) && (((*reg1 ^ *dest) & 0x80) != 0);
            } break;
            case MULB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                *dest = *reg1 * *reg2;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = false;
                overflow = carry;
            } break;
            case DIVB: {
                u8 *dest = fetch_register_u8(cycles);
                u8 *reg1 = fetch_register_u8(cycles);
                u8 *reg2 = fetch_register_u8(cycles);
                *dest = *reg1 / *reg2;

                zero = (*dest == 0);
                sign = (*dest & 0x80) != 0;
                carry = false;
                overflow = false;
            } break;
			case AND: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					u8 *reg2 = fetch_register_u8(cycles);
					*dest = *reg1 & *reg2;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					u16 *reg2 = fetch_register_u16(cycles);
					*dest = *reg1 & *reg2;
				} break;
				}
			} break;
			case OR: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					u8 *reg2 = fetch_register_u8(cycles);
					*dest = *reg1 | *reg2;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					u16 *reg2 = fetch_register_u16(cycles);
					*dest = *reg1 | *reg2;
				} break;
				}
			} break;
			case XOR: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					u8 *reg2 = fetch_register_u8(cycles);
					*dest = *reg1 ^ *reg2;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					u16 *reg2 = fetch_register_u16(cycles);
					*dest = *reg1 ^ *reg2;
				} break;
				}
			} break;
			case NOT: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					*dest = ~*reg1;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					*dest = ~*reg1;
				} break;
				}
			} break;
			case SHL: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					u8 *reg2 = fetch_register_u8(cycles);
					*dest = *reg1 << *reg2;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					u16 *reg2 = fetch_register_u16(cycles);
					*dest = *reg1 << *reg2;
				} break;
				}
			} break;
			case SHR: {
				u8 mode = fetch_byte(cycles);
				switch (mode) {
				case 0xA0: {
					u8 *dest = fetch_register_u8(cycles);
					u8 *reg1 = fetch_register_u8(cycles);
					u8 *reg2 = fetch_register_u8(cycles);
					*dest = *reg1 >> *reg2;
				} break;
				case 0xA1: {
					u16 *dest = fetch_register_u16(cycles);
					u16 *reg1 = fetch_register_u16(cycles);
					u16 *reg2 = fetch_register_u16(cycles);
					*dest = *reg1 >> *reg2;
				} break;
				}
			} break;
			case EQU: {
				equal = *fetch_register_u16(cycles) == *fetch_register_u16(cycles);
			} break;
			case JZ: {
				u16 value = 0xD000 + fetch_word(cycles);
				if (equal == 0) {
					pc = value;
				}
			} break;
			case JNZ: {
				u16 value = 0xD000 + fetch_word(cycles);
				if (equal != 0) {
					pc = value;
				}
			} break;
			case JMP: {
				pc = 0xD000 + fetch_word(cycles);
			} break;
			case HLT: {
				cycles = 0;
			} break;
			default: {
				std::cerr << "Invalid CPU instruction: "
					<< "0x" << std::hex << std::uppercase << static_cast<int>(inst) << std::endl;
				exit(1);
			} break;
			}
		}
	}
private:
	u8 fetch_byte(size_t &cycles) {
		u8 byte = memory.data[pc];
		pc++;

		cycles--;
		return byte;
	}

	u16 fetch_word(size_t &cycles) {
		u8 low_byte = memory.data[pc];
		u8 high_byte = memory.data[pc +  1];
		pc += 2;

		cycles--;
		return ((u16)low_byte << 8) | (u16)high_byte;
	}

	u16 *fetch_register_u16(size_t &cycles) {
		u8 code = memory.data[pc];
		pc++;
		cycles--;
		
		switch (code) {
		case R0: return &r0;
		case R1: return &r1;
		case R2: return &r2;
		case R3: return &r3;
		case RA: return &ra;
		case RI: return &ri;
		default: {
			std::cerr << "Register code does not exist: " << code << std::endl;
			exit(1);
		}
		}
	}

	u8 *fetch_register_u8(size_t &cycles) {
		u8 code = memory.data[pc];
		pc++;
		cycles--;
		
		switch (code) {
		case B0: return &b0;
		case B1: return &b1;
		case B2: return &b2;
		case B3: return &b3;
		default: {
			std::cerr << "Register code does not exist: " << code << std::endl;
			exit(1);
		}
		}
	}
public:
	enum Inst {
		LR0		= 0xA0,
		LR1		= 0xA1,
		LR2		= 0xA2,
		LR3		= 0xA3,

		LB0		= 0xA4,
		LB1		= 0xA5,
		LB2		= 0xA6,
		LB3		= 0xA7,

		LDA		= 0xA8,
		LDI		= 0xA9,

		PUSH	= 0xAA,
		POP		= 0xAB,

		STB		= 0xAC,
		STW		= 0xAD,
		LDB		= 0xAE,
		LDW		= 0xAF,
		
		ADD		= 0xB1,
		ADC		= 0xB2,
		SUB		= 0xB3,
		SBB		= 0xB4,
		MUL		= 0xB5,
		DIV		= 0xB6,
		
		ADDB	= 0xB7,
		ADCB	= 0xB8,
		SUBB	= 0xB9,
		SBBB	= 0xBA,
		MULB	= 0xBB,
		DIVB	= 0xBC,

		EQU		= 0xC0,
		JZ		= 0xC1,
		JNZ		= 0xC2,
		JMP		= 0xC3,

		AND		= 0xE0,
		OR		= 0xE1,
		XOR		= 0xE2,
		NOT		= 0xE3,
		SHL		= 0xE4,
		SHR		= 0xE5,

		HLT		= 0xFF,
	};

	enum RegCode {
		R0		= 0xA0,
		R1		= 0xA1,
		R2		= 0xA2,
		R3		= 0xA3,

		RA		= 0xA4,
		RI		= 0xA5,
		
		B0		= 0xB5,
		B1		= 0xB6,
		B2		= 0xB7,
		B3		= 0xB8
	};

	enum ValueCode {
		BYTE	= 0xA0,
		WORD	= 0xA1,
	};
public:
	u16 pc;
	u16 sp;
	u16 fbs;
	
	// 16-bit registers
	u16 r0;
	u16 r1;
	u16 r2;
	u16 r3;

	// Special Purpose Accumulator Register
	u16 ra;

	// Special Purpose All-Intermediate Register
	// Typically recommended for memory addresses
	u16 ri;
	
	// 8-bit registers
	u8 b0;
	u8 b1;
	u8 b2;
	u8 b3;

	u8 equal		: 1;

	u8 zero			: 1;
	u8 decimal		: 1;
	u8 sign			: 1;
	u8 carry		: 1;
	u8 overflow		: 1;
	u8 interrupt	: 1;
	u8 breakf		: 1;
private:
	Memory memory;
};

int main() {
	Memory memory {};
	CPU cpu(memory);
	cpu.reset();

	std::vector<u8> program;
	std::ifstream file("C:\\Dev\\R828em\\output.bin", std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file!" << std::endl;
		exit(1);
	}

	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

	program.resize(fileSize);

	if (!file.read(reinterpret_cast<char*>(program.data()), fileSize)) {
        std::cerr << "Failed to read file" << std::endl;
        exit(1);
    }

	file.close();

	for (int i = 0; i < program.size(); i++) {
		cpu.load_addr(cpu.pc + i, program[i]);
	}

	size_t cycles = std::numeric_limits<double>::infinity();
	while (cycles > 0) {
		cpu.execute(cycles);
	}

	std::cout << "R0: " << static_cast<i16>(cpu.r0) << std::endl;
	std::cout << "R1: " << static_cast<i16>(cpu.r1) << std::endl;
	std::cout << "R2: " << static_cast<i16>(cpu.r2) << std::endl;
	std::cout << "R3: " << static_cast<i16>(cpu.r3) << std::endl;
	std::cout << "RA: " << static_cast<i16>(cpu.ra) << std::endl;
	std::cout << "RI: " << static_cast<i16>(cpu.ri) << std::endl;
	std::cout << "B0: " << static_cast<i16>(cpu.b0) << std::endl;
	std::cout << "B1: " << static_cast<i16>(cpu.b1) << std::endl;
	std::cout << "B2: " << static_cast<i16>(cpu.b2) << std::endl;
	std::cout << "B3: " << static_cast<i16>(cpu.b3) << std::endl;
	std::cout << "EQUAL: " << static_cast<u16>(cpu.equal) << std::endl;

	return 0;
}