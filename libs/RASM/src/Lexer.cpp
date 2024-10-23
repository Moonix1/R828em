#include "Lexer.h"
#include "Log/Log.h"

namespace RASM {
	
	Lexer::Lexer(std::vector<Token> &tokens, const std::string& source) {
		m_Input = source;
		m_Pos = 0;

		while (m_Pos < m_Input.size()) {
			tokens.push_back(FetchToken());
		}

		for (Token &tok : tokens) {
			INFO("{0} -> {1}", TokenTypeToString(tok.type), tok.value);
		}
	}

	Token Lexer::FetchToken() {
		while (m_Pos < m_Input.size()) {
			char current = m_Input[m_Pos];
			if (std::isspace(current)) { m_Pos++; continue; }

			if (std::isalpha(current)) {
				return LexKI();
			}

			if (std::isdigit(current)) {
				return LexNumber();
			}

			switch (current) {
			case '(': m_Pos++; return { .type = LPAREN, .value = std::string(1, current) };
			case ')': m_Pos++; return { .type = RPAREN, .value = std::string(1, current) };
			case '[': m_Pos++; return { .type = LBRACE, .value = std::string(1, current) };
			case ']': m_Pos++; return { .type = RBRACE, .value = std::string(1, current) };
			case '{': m_Pos++; return { .type = LCURLYBRACE, .value = std::string(1, current) };
			case '}': m_Pos++; return { .type = RCURLYBRACE, .value = std::string(1, current) };
			case ':': m_Pos++; return { .type = COLON, .value = std::string(1, current) };
			case ';': m_Pos++; return { .type = SEMICOLON, .value = std::string(1, current) };
			case '_': m_Pos++; return { .type = UNDERSCORE, .value = std::string(1, current) };
			}

			m_Pos++;
		}
	}

	Token Lexer::LexKI() {
		int start = m_Pos;
		while (m_Pos < m_Input.size() && std::isalnum(m_Input[m_Pos])) { m_Pos++; }
		std::string value = m_Input.substr(start, m_Pos - start);

		if (value == "LR0"
			|| value == "LR1"
			|| value == "LR2"
			|| value == "LR3"
			|| value == "LB0"
			|| value == "LB1"
			|| value == "LB2"
			|| value == "LB3"
			|| value == "LDW"
			|| value == "HLT")
			return { .type = OPCODE, .value = value };

		if (value == "SECTION")
			return { .type = SECTION, .value = value };

		return { .type = IDENT, .value = value };
	}

	Token Lexer::LexNumber() {
		int start = m_Pos;
		while (m_Pos < m_Input.size() && std::isdigit(m_Input[m_Pos]) || std::isalpha(m_Input[m_Pos])) { m_Pos++; }
		std::string value = m_Input.substr(start, m_Pos - start);

		if (value[1] == 'x' || value[1] == 'X')
			return { .type = HEX, .value = value };

		return { .type = NUMBER, .value = value };
	}

}