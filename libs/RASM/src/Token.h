#pragma once

#include <string>
#include <memory>

#include "Log/Log.h"

namespace RASM {

	enum TokenType {
		OPCODE,

		SECTION,

		IDENT,

		NUMBER,
		HEX,

		LPAREN,
		RPAREN,
		LBRACE,
		RBRACE,
		LCURLYBRACE,
		RCURLYBRACE,
		COLON,
		SEMICOLON,
		UNDERSCORE,

		UNKNOWN,
	};

	struct Token {
		TokenType type;
		std::string value;
	};

	inline std::string TokenTypeToString(TokenType type) {
		std::unordered_map<TokenType, std::string> tokenTypeToString = {
			{ TokenType::OPCODE, "opcode" },
			{ TokenType::SECTION, "section" },
			{ TokenType::NUMBER, "number" },
			{ TokenType::HEX, "hex" },
			{ TokenType::LPAREN, "lparen" },
			{ TokenType::RPAREN, "rparen" },
			{ TokenType::LBRACE, "lbrace" },
			{ TokenType::RBRACE, "rbrace" },
			{ TokenType::LCURLYBRACE, "lcurlybrace" },
			{ TokenType::RCURLYBRACE, "rcurlybrace" },
			{ TokenType::COLON, "colon" },
			{ TokenType::SEMICOLON, "semicolon" },
			{ TokenType::UNDERSCORE, "underscore" },
			{ TokenType::IDENT, "ident" },
		};

		return tokenTypeToString[type];
	}

	static Token FetchToken(std::vector<Token> &tokens, int &pos) {
		Token result = tokens[pos];
		pos++;
		return result;
	}

	static Token ExpectToken(std::vector<Token> &tokens, TokenType expect, int &pos) {
		Token token = FetchToken(tokens, pos);
		if (token.type == expect) { /*ERROR("Unexpected token, expected: {0} got: {1}",
			TokenTypeToString(expect), TokenTypeToString(token.type)); exit(1);*/ }

		return token;
	}

}