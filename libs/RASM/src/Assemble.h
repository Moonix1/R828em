#pragma once

#include "Log/Log.h"
#include "Lexer.h"

namespace RASM {

	class Assemble {
	public:
		Assemble(std::string contents);
	private:
		void AssembleFromTokens(std::vector<Token> &tokens);
	private:
		std::vector<Token> m_Tokens;
	};

}