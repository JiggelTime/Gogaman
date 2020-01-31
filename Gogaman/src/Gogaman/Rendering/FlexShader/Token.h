#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token
		{
			enum class Type : uint8_t
			{
				Number,
				Identifier,
				String,
				Colon,
				Semicolon,
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
			Type             type;
		};
	}
}