#pragma once

constexpr uint8_t LR0		= 0xA0;
constexpr uint8_t LR1		= 0xA1;
constexpr uint8_t LR2		= 0xA2;
constexpr uint8_t LR3		= 0xA3;

constexpr uint8_t LB0		= 0xA4;
constexpr uint8_t LB1		= 0xA5;
constexpr uint8_t LB2		= 0xA6;
constexpr uint8_t LB3		= 0xA7;

constexpr uint8_t LDA		= 0xA8;
constexpr uint8_t LDI		= 0xA9;

constexpr uint8_t PUSH		= 0xAA;
constexpr uint8_t POP		= 0xAB;

constexpr uint8_t STB		= 0xAC;
constexpr uint8_t STW		= 0xAD;
constexpr uint8_t LDB		= 0xAE;
constexpr uint8_t LDW		= 0xAF;
		
constexpr uint8_t ADD		= 0xB1;
constexpr uint8_t ADC		= 0xB2;
constexpr uint8_t SUB		= 0xB3;
constexpr uint8_t SBB		= 0xB4;
constexpr uint8_t MUL		= 0xB5;
constexpr uint8_t DIV		= 0xB6;
		
constexpr uint8_t ADDB		= 0xB7;
constexpr uint8_t ADCB		= 0xB8;
constexpr uint8_t SUBB		= 0xB9;
constexpr uint8_t SBBB		= 0xBA;
constexpr uint8_t MULB		= 0xBB;
constexpr uint8_t DIVB		= 0xBC;

constexpr uint8_t EQU		= 0xC0;
constexpr uint8_t JZ		= 0xC1;
constexpr uint8_t JNZ		= 0xC2;
constexpr uint8_t JMP		= 0xC3;

constexpr uint8_t AND		= 0xE0;
constexpr uint8_t OR		= 0xE1;
constexpr uint8_t XOR		= 0xE2;
constexpr uint8_t NOT		= 0xE3;
constexpr uint8_t SHL		= 0xE4;
constexpr uint8_t SHR		= 0xE5;

constexpr uint8_t HLT		= 0xFF;

constexpr uint8_t R0		= 0xA0;
constexpr uint8_t R1		= 0xA1;
constexpr uint8_t R2		= 0xA2;
constexpr uint8_t R3		= 0xA3;

constexpr uint8_t RA		= 0xA4;
constexpr uint8_t RI		= 0xA5;

constexpr uint8_t B0		= 0xB5;
constexpr uint8_t B1		= 0xB6;
constexpr uint8_t B2		= 0xB7;
constexpr uint8_t B3		= 0xB8;