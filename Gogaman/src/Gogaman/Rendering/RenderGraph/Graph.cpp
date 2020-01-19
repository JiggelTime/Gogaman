#include "pch.h"
#include "Graph.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		void Graph::SetOutputTextureName(const std::string &name)
		{
			GM_DEBUG_ASSERT(m_OutputTextureName.empty(), "Failed to set graph output texture name | Output texture name is already set");

			m_OutputTextureName = name;
		}

		std::pair<Stage::Type, Graph::StageIndex> Graph::StageIndexData(const StageIndex vertexIndex) const
		{
			if(vertexIndex < m_ComputeStages.size())
				return { Stage::Type::Compute, vertexIndex };

			return { Stage::Type::Render, vertexIndex - m_ComputeStages.size() };
		}

		Graph::StageIndex Graph::VertexIndex(const Stage::Type stageType, const StageIndex stageIndex) const
		{
			if(stageType == Stage::Type::Compute)
				return stageIndex;

			return stageIndex + m_ComputeStages.size();
		}
	}
}