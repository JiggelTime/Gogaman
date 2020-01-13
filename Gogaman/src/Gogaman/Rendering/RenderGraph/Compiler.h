#pragma once

namespace Gogaman
{
	namespace RenderGraph
	{
		class Graph;

		class ExecutableGraph;

		class Compiler
		{
		public:
			static std::unique_ptr<ExecutableGraph> Compile(Graph &&graph);
		private:
			Compiler()  = delete;
			~Compiler() = delete;
		};
	}
}