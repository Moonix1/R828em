#include "Assemble.h"

#include <fstream>

#include "Defines.h"

namespace RASM {

	uint16_t StringToU16H(std::string number) {
		char *end;
		uint16_t value = (uint16_t)std::stoul(number, nullptr, 16);

		return value;
	}

	void PushU16(std::vector<uint8_t> &binSource, uint16_t number) {
		binSource.push_back((number >> 8) & 0xFF);
		binSource.push_back(number & 0xFF);
	}

	void PushNumberU16(std::vector<uint8_t> &binSource, std::string number) {
		char *end;
		uint16_t value = (uint16_t)std::strtol(number.c_str(), &end, 10);
		binSource.push_back((value >> 8) & 0xFF);
		binSource.push_back(value & 0xFF);
	}
	
	void PushNumberU8(std::vector<uint8_t> &binSource, std::string number) {
		char *end;
		uint8_t value = (uint8_t)std::strtol(number.c_str(), &end, 10);
		binSource.push_back(value);
	}

	uint8_t RegToU8(std::string reg) {
		if (reg == "R0") return R0;
		if (reg == "R1") return R1;
		if (reg == "R2") return R2;
		if (reg == "R3") return R3;
		if (reg == "RA") return RA;
		if (reg == "RI") return RI;
		if (reg == "B0") return B0;
		if (reg == "B1") return B1;
		if (reg == "B2") return B2;
		if (reg == "B3") return B3;
	}

	static Token NextToken(int &iter, std::vector<Token> &tokens) {
		if (iter + 1 >= tokens.size()) {
			ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
			exit(1);
		} else iter++;
		return tokens[iter];
	}

	static Token ExpectNextToken(int &iter, std::vector<Token> &tokens, TokenType expect) {
		if (iter + 1 >= tokens.size()) {
			ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
			exit(1);
		} else iter++;

		if (tokens[iter].type != expect) {
			ERROR("Expected: {0}, got: {1}", TokenTypeToString(expect), TokenTypeToString(tokens[iter].type));
			exit(1);
		}
		return tokens[iter];
	}

	static void ExpectNextTokenV(int &iter, std::vector<Token> &tokens, TokenType expect) {
		if (iter + 1 >= tokens.size()) {
			ERROR("NextToken: Out of range! last token: {0}", TokenTypeToString(tokens[iter].type));
			exit(1);
		}

		if (tokens[iter + 1].type == expect) {
			return;
		} else {
			ERROR("Expected: {0}, got: {1}", TokenTypeToString(expect), TokenTypeToString(tokens[iter].type));
			exit(1);
		}
	}

	Assemble::Assemble(std::string contents) {
		std::vector<Token> tokens;

		Lexer lexer(tokens, contents);
		m_Tokens = tokens;

		AssembleFromTokens(tokens);
	}

	void Assemble::AssembleFromTokens(std::vector<Token>& tokens) {
		std::vector<uint8_t> binSource;
		
		int tokIndex = 0;
		while (tokIndex < tokens.size()) {
			Token tok = tokens[tokIndex];
			switch (tok.type) {
			case ORG: {
				Token value = ExpectNextToken(tokIndex, tokens, HEX);
				m_PointAddr = StringToU16H(value.value);
			} break;
			case IDENT: {
				ExpectNextTokenV(tokIndex, tokens, COLON);
				m_Labels.push_back(Label { .name = tokens[tokIndex].value, .addr = m_PointAddr });
				NextToken(tokIndex, tokens);
			} break;
			case OPCODE: {
				if (tok.value == "LR0") {
					binSource.push_back(LR0);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LR1") {
					binSource.push_back(LR1);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LR2") {
					binSource.push_back(LR2);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LR3") {
					binSource.push_back(LR3);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LB0") {
					binSource.push_back(LB0);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU8(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LB1") {
					binSource.push_back(LB1);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU8(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LB2") {
					binSource.push_back(LB2);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU8(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LB3") {
					binSource.push_back(LB3);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU8(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LDA") {
					binSource.push_back(LDA);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "LDI") {
					binSource.push_back(LDI);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
					m_PointAddr++;
				} else if (tok.value == "JMP") {
					binSource.push_back(JMP);
					Token value = ExpectNextToken(tokIndex, tokens, IDENT);
					bool found = false;
					for (Label label : m_Labels) {
						INFO("label: {0}", label.name);
						if (label.name == value.value) {
							PushU16(binSource, label.addr);
							found = true;
						}
					}

					if (!found) {
						ERROR("Invalid label: {0}", value.value);
						exit(1);
					}
				} else if (tok.value == "ADD" ) {
					binSource.push_back(ADD);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "ADC") {
					binSource.push_back(ADC);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "SUB") {
					binSource.push_back(SUB);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "SBB") {
					binSource.push_back(SBB);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "MUL") {
					binSource.push_back(MUL);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "DIV") {
					binSource.push_back(DIV);
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					binSource.push_back(RegToU8(ExpectNextToken(tokIndex, tokens, REG).value));
					m_PointAddr++;
				} else if (tok.value == "HLT") { binSource.push_back(HLT); m_PointAddr++; }
			} break;
			}

			tokIndex++;
		}

		std::string name = "output.bin";
		std::ofstream outfile(name, std::ios::binary);
		if (!outfile) {
			ERROR("Error opening file: {0}", name);
			exit(1);
		}
		
		for (uint8_t byte : binSource) {
			outfile.write(reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
		}

		outfile.close();
	}

}