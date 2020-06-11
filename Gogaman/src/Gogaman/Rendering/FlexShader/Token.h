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
				Number,
				String,
				Identifier,
				Keyword,
				Comma,
				Dot,
				Slash,
				//Backslash,
				Colon,
				Semicolon,
				Percent,
				Asterisk,
				LeftParenthesis,
				RightParenthesis,
				LeftSquareBracket,
				RightSquareBracket,
				LeftBrace,
				RightBrace,
				Equal,
				Less,
				Greater,
				Plus,
				Minus
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
			"Comma",
			"Dot",
			"Slash",
			//"Backslash",
			"Colon",
			"Semicolon",
			"Percent",
			"Asterisk",
			"LeftParenthesis",
			"RightParenthesis",
			"LeftSquareBracket",
			"RightSquareBracket",
			"LeftBrace",
			"RightBrace",
			"Equal",
			"Less",
			"Greater",
			"Plus",
			"Minus"
			};

			return std::string(names[(uint8_t)type]);
		}

		inline std::string GetTokenString(const Token &token) { return GetTokenTypeString(token.type).append(": ").append(token.lexeme); }
	}
}