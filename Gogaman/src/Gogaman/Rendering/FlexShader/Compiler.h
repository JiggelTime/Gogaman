#pragma once

namespace Gogaman
{
	namespace FlexShader
	{
		namespace AST { namespace Node { struct Abstract; } }

		AST::Node::Abstract *Compile(const std::string &source);
	}
}