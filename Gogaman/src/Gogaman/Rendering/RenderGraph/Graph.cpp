#include "pch.h"
#include "Graph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void Graph::CreateStage(const Stage::InitializeFunction &initialize, const Stage::ExecuteFunction &execute)
		{
			Stage::Index index = m_DirectedGraph.CreateVertex();
			m_Stages.emplace_back(index, initialize, execute);
		}
	}
}