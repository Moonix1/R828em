#pragma once

#include <vector>

#include "Token.h"

namespace RASM {

	class Lexer {
	public:
		Lexer(std::vector<Token> &tokens, const std::string &source);
	private:
		Token FetchToken();

		Token LexKI();
		Token LexNumber();
	private:
		std::string m_Input;
		int m_Pos;
	};

}