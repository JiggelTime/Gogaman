#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphStage.h"
#include "RenderGraph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		//class Stage;
		//class TextureResource;
		//class Graph;
		
		class StageBuilder
		{
		public:
			inline StageBuilder(Graph *graph, Stage *stage)
				: m_Graph(graph), m_Stage(stage)
			{}

			inline ~StageBuilder() = default;

			VirtualTexture &CreateTexture(const std::string &name, TextureDescriptor &&descriptor);

			VirtualTexture &ReadTexture(const std::string &name);

			VirtualTexture &WriteTexture(const std::string &name, const std::string &outputName);
		private:
			Graph *m_Graph;
			Stage *m_Stage;
		};
	}
}