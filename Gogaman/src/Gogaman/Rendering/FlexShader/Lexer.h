#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token;

		std::vector<Token> Lex(const std::string &source);
	}
}