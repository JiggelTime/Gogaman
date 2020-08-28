#pragma once

#define GM_FLEX_SHADER_TOKEN_TYPE_COUNT 22

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token
		{
			enum class Type : uint8_t
			{
				//Literals
				Number,
				String,

				//Names
				Identifier,
				Keyword,

				//Separators
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
				Greater,

				//Miscellaneous
				Comma,
				Dot,
				Colon
			};

			std::string_view lexeme;
			uint16_t         line;
			Type             type;
		};

		inline std::string GetTokenTypeString(const Token::Type &type)
		{
			const std::string names[]
			{
			"Number",
			"String",
			"Identifier",
			"Keyword",
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
			"Greater",
			"Comma",
			"Dot",
			"Colon"
			};

			return std::string(names[(uint8_t)type]);
		}

		inline std::string GetTokenString(const Token &token) { return GetTokenTypeString(token.type).append(": ").append(token.lexeme); }
	}
}