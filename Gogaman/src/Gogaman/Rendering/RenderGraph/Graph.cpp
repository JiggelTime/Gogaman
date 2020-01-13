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
			StageIndex index = vertexIndex;
			if(index < m_ComputeStages.size())
				return { Stage::Type::Compute, index };

			index -= m_ComputeStages.size();
			if(index < m_PrerecordedComputeStages.size())
				return { Stage::Type::PrerecordedCompute, index };

			index -= m_PrerecordedComputeStages.size();
			if(index < m_RenderStages.size())
				return { Stage::Type::Render, index };
			
			index -= m_RenderStages.size();
			return { Stage::Type::PrerecordedRender, index };
		}

		Graph::StageIndex Graph::VertexIndex(const Stage::Type stageType, const StageIndex stageIndex) const
		{
			switch(stageType)
			{
			case Stage::Type::Compute:
				return stageIndex;
			case Stage::Type::PrerecordedCompute:
				return stageIndex + m_ComputeStages.size();
			case Stage::Type::Render:
				return stageIndex + m_ComputeStages.size() + m_PrerecordedComputeStages.size();
			case Stage::Type::PrerecordedRender:
				return stageIndex + m_ComputeStages.size() + m_PrerecordedComputeStages.size() + m_RenderStages.size();
			}
		}
	}
}