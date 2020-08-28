#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		struct Token;
		namespace AST { namespace Node { struct Abstract; } }

		//Belongs in a Compiler class with Compile() function
		inline std::unordered_set<std::string> g_ParsedComponents;

		AST::Node::Abstract *Parse(std::vector<Token> &&tokens);
	}
}