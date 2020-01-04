#pragma once

#include "Stage.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class Stage;

		class ExecutableGraph
		{
		public:
			ExecutableGraph(std::vector<Stage> &&executionQueue)
				: m_ExecutionQueue(std::move(executionQueue))
			{}

			inline void Execute()
			{
				for(Stage &i : m_ExecutionQueue)
				{
					i.Execute();
				}
			}
		private:
			std::vector<Stage> m_ExecutionQueue;
		};
	}
}