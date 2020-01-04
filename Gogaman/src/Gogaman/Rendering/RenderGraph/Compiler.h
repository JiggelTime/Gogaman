#pragma once

namespace Gogaman
{
	namespace RenderGraph
	{
		class Graph;

		class ExecutableGraph;

		//Change to global function?
		class Compiler
		{
		public:
			static const ExecutableGraph &Compile(Graph &&graph);
		private:
			Compiler()  = delete;
			~Compiler() = delete;
		};
	}
}