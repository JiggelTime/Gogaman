#pragma once

#include "Gogaman/Utilities/Algorithm/DirectedGraph.h"

#include "Stage.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class Compiler;

		class Graph
		{
		public:
			Graph()  = default;
			~Graph() = default;

			void CreateStage(const Stage::InitializeFunction &initialize, const Stage::ExecuteFunction &execute);

			inline void SetOutputTexture(const std::string &name) { m_OutputTextureName = name; }
			inline constexpr const std::string &GetOutputTextureName() const { return m_OutputTextureName; }
		private:
			DirectedGraph<Stage::Index> m_DirectedGraph;

			std::vector<Stage> m_Stages;

			std::string m_OutputTextureName;
		private:
			friend Compiler;
		};
	}
}