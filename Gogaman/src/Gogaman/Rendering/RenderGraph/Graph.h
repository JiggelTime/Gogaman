#pragma once

#include "Stage.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class Compiler;

		class Graph
		{
		public:
			using StageIndex = uint8_t;
		public:
			Graph()
				: m_OutputTextureName("")
			{}

			~Graph() = default;

			inline ComputeStage &CreateComputeStage(ComputeStage::StateData &&state) { return m_ComputeStages.emplace_back(std::move(state)); }
			inline RenderStage  &CreateRenderStage(RenderStage::StateData &&state)   { return m_RenderStages.emplace_back(std::move(state));  }

			void SetOutputTextureName(const std::string &name);
			inline constexpr const std::string &GetOutputTextureName() const { return m_OutputTextureName; }
		private:
			std::pair<Stage::Type, StageIndex> StageIndexData(const StageIndex vertexIndex) const;
			StageIndex VertexIndex(const Stage::Type stageType, const StageIndex stageIndex) const;
		private:
			std::vector<ComputeStage> m_ComputeStages;
			std::vector<RenderStage>  m_RenderStages;

			std::string m_OutputTextureName;
		private:
			friend Compiler;
		};
	}
}