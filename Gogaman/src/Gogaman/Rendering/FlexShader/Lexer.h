#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token;

		/*
		enum class AttributeFlag : uint8_t
		{
			Qualified = 1 << 0,
			Reserved  = 1 << 1
		};

		using AttributeFlags = uint8_t;
		*/

		std::vector<Token> Tokenize(const std::string &source);
	}
}