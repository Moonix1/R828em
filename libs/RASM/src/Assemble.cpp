#include "Assemble.h"

#include <fstream>

#include "Defines.h"

namespace RASM {

	void PushNumberU16(std::vector<uint8_t> &binSource, std::string number) {
		char *end;
		uint16_t value = (uint16_t)std::strtol(number.c_str(), &end, 10);
		binSource.push_back((value >> 8) & 0xFF);
		binSource.push_back(value & 0xFF);
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
			case IDENT: {
				ExpectNextToken(tokIndex, tokens, COLON);
			} break;
			case OPCODE: {
				if (tok.value == "LR0") {
					binSource.push_back(LR0);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LR1") {
					binSource.push_back(LR1);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LR2") {
					binSource.push_back(LR2);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LR3") {
					binSource.push_back(LR3);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LB0") {
					binSource.push_back(LB0);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LB1") {
					binSource.push_back(LB1);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LB2") {
					binSource.push_back(LB2);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "LB3") {
					binSource.push_back(LB3);
					Token value = ExpectNextToken(tokIndex, tokens, NUMBER);
					PushNumberU16(binSource, value.value);
				} else if (tok.value == "HLT") binSource.push_back(HLT);
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