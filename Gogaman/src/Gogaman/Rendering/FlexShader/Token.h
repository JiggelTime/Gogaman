#pragma once

#define GM_FLEX_SHADER_TOKEN_TYPE_COUNT 24

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token
		{
			enum class Type : uint8_t
			{
				//Literals
				BooleanLiteral,
				IntegerLiteral,
				FloatingPointLiteral,
				StringLiteral,

				//Names
				Identifier,
				Keyword,

				//Delimiters
				Comma,
				Dot,
				Colon,
				Semicolon,
				LeftParenthesis,
				RightParenthesis,
				LeftBrace,
				RightBrace,
				LeftSquareBracket,
				RightSquareBracket,

				//Operators
				Plus,
				Minus,
				Asterisk,
				Slash,
				Percent,
				Equal,
				Less,
				Greater
			};

			std::string_view lexeme;
			uint16_t         line;
			Type             type;
		};

		inline std::string GetTokenTypeString(const Token::Type &type)
		{
			const std::string names[]
			{
				"BooleanLiteral",
				"IntegerLiteral",
				"FloatingPointLiteral",
				"StringLiteral",
				"Identifier",
				"Keyword",
				"Comma",
				"Dot",
				"Colon",
				"Semicolon",
				"LeftParenthesis",
				"RightParenthesis",
				"LeftBrace",
				"RightBrace",
				"LeftSquareBracket",
				"RightSquareBracket",
				"Plus",
				"Minus",
				"Asterisk",
				"Slash",
				"Percent",
				"Equal",
				"Less",
				"Greater"
			};

			return std::string(names[(uint8_t)type]);
		}

		inline std::string GetTokenString(const Token &token) { return GetTokenTypeString(token.type).append(": ").append(token.lexeme); }
	}
}