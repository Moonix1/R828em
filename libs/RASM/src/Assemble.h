#pragma once

#include "Log/Log.h"
#include "Lexer.h"

namespace RASM {

	struct Label {
		std::string name;
		uint16_t addr;
	};

	class Assemble {
	public:
		Assemble(std::string contents);
	private:
		void AssembleFromTokens(std::vector<Token> &tokens);
	private:
		std::vector<Token> m_Tokens;
		std::vector<Label> m_Labels;
		
		uint16_t m_PointAddr = 0x0000;
	};

}