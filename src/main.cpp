#include <iostream>
#include <vector>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t i8;
typedef int16_t i16;

#define MEMORY_CAPACITY 1024 * 64
struct Memory {
	u16 data[MEMORY_CAPACITY];
};

struct CPU {
public:
	CPU(Memory mem)
		: memory(mem) {}

	void reset() {
		pc = 0xD000;
		sp = 0xB000;

		r0 = 0x0000;
		r1 = 0x0000;
		r2 = 0x0000;
		r3 = 0x0000;

		br0 = 0x00;
		br1 = 0x00;
		br2 = 0x00;
		br3 = 0x00;

		equal		= 0;
		zero		= 0;
		decimal		= 0;
		sign		= 0;
		carry		= 0;
		overflow	= 0;
		interrupt	= 0;
		breakf		= 0;

		for (int i = 0; i < MEMORY_CAPACITY; i++) {
			memory.data[i] = 0x00;
		}
	}

	void load_addr(u16 addr, u8 byte_value) {
		memory.data[addr] = byte_value;
	}

	void execute(size_t cycles) {
		while (cycles > 0) {
			u8 inst = fetch_byte(cycles);

			switch (inst) {
			case LDR0: {
				u16 value = fetch_word(cycles);
				r0 = value;
			} break;
			case LDR1: {
				u16 value = fetch_word(cycles);
				r1 = value;
			} break;
			case LDR2: {
				u16 value = fetch_word(cycles);
				r2 = value;
			} break;
			case LDR3: {
				u16 value = fetch_word(cycles);
				r3 = value;
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
					u16 low_byte = fetch_byte(cycles);
					u16 high_byte = fetch_byte(cycles);
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

				*dest = ((u16)memory.data[*addr]) | (u16)memory.data[*addr + 1];
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
		case BR0: return &br0;
		case BR1: return &br1;
		case BR2: return &br2;
		case BR3: return &br3;
		default: {
			std::cerr << "Register code does not exist: " << code << std::endl;
			exit(1);
		}
		}
	}
public:
	enum Inst {
		LDR0	= 0xA0,
		LDR1	= 0xA1,
		LDR2	= 0xA2,
		LDR3	= 0xA3,

		PUSH	= 0xA4,
		POP		= 0xA5,

		STB		= 0xA6,
		STW		= 0xA7,
		LDB		= 0xA8,
		LDW		= 0xA9,
		
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

		HLT		= 0xFF,
	};

	enum RegCode {
		R0		= 0xA0,
		R1		= 0xA1,
		R2		= 0xA2,
		R3		= 0xA3,
		
		BR0		= 0xA4,
		BR1		= 0xA5,
		BR2		= 0xA6,
		BR3		= 0xA7
	};

	enum ValueCode {
		BYTE	= 0xA0,
		WORD	= 0xA1,
	};
public:
	u16 pc;
	u16 sp;

	// 16-bit registers
	u16 r0;
	u16 r1;
	u16 r2;
	u16 r3;
	
	// 8-bit registers
	u8 br0;
	u8 br1;
	u8 br2;
	u8 br3;

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

	std::vector<u8> program = {
		cpu.LDR0, 0x00, 0x52,
		cpu.LDR1, 0x00, 0x52,
		cpu.EQU, cpu.R0, cpu.R1,
		cpu.PUSH, cpu.WORD, 0x32, 0x00,
		cpu.PUSH, cpu.WORD, 0x01, 0x00,
		cpu.PUSH, cpu.BYTE, 0xFF,
		cpu.POP, cpu.BYTE, cpu.BR0,
		cpu.POP, cpu.WORD, cpu.R2,
		cpu.POP, cpu.WORD, cpu.R3,
		cpu.LDR3, 0x00, 0x00,
		cpu.STB, cpu.R3, 0xA0, 0x02,
		cpu.LDR3, 0x00, 0x01,
		cpu.STW, cpu.R3, 0xA0, 0x00, 0x32,
		cpu.LDR3, 0x00, 0x00,
		cpu.LDB, cpu.BR1, cpu.R3,
		cpu.LDR3, 0x00, 0x01,
		cpu.LDW, cpu.R3, cpu.R3,
		cpu.HLT,
	};

	for (int i = 0; i < program.size(); i++) {
		cpu.load_addr(cpu.pc + i, program[i]);
	}

	cpu.execute(std::numeric_limits<double>::infinity());

	std::cout << "R0: " << static_cast<i16>(cpu.r0) << std::endl;
	std::cout << "R1: " << static_cast<i16>(cpu.r1) << std::endl;
	std::cout << "R2: " << static_cast<i16>(cpu.r2) << std::endl;
	std::cout << "R3: " << static_cast<i16>(cpu.r3) << std::endl;
	std::cout << "BR0: " << static_cast<i16>(cpu.br0) << std::endl;
	std::cout << "BR1: " << static_cast<i16>(cpu.br1) << std::endl;
	std::cout << "BR2: " << static_cast<i16>(cpu.br2) << std::endl;
	std::cout << "BR3: " << static_cast<i16>(cpu.br3) << std::endl;
	std::cout << "EQUAL: " << static_cast<u16>(cpu.equal) << std::endl;
}