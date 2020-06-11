#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token;

		std::vector<Token> Tokenize(const std::string &source);
	}
}