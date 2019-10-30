#pragma once

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include "RenderGraphTypeAlias.h"
#include "RenderGraphStage.h"
#include "RenderGraphResource.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class StageBuilder;

		class Graph
		{
		public:
			inline Graph()  = default;
			inline ~Graph() = default;

			void SetBackBuffer(const std::string &name);
			
			Stage &CreateStage();

			void Compile();

			void Log() const;
		private:
			void GenerateDependencies(const Stage &stage);

			TextureResource &GetTextureResource(const std::string &name);
		private:
			friend StageBuilder;

			std::vector<std::vector<StageIndex>>                  m_StageDependencies;
			std::vector<StageIndex>                               m_StageSchedule;
			std::vector<Stage>                                    m_Stages;
			std::string                                           m_BackBuffer;
			std::unordered_map<std::string, TextureResourceIndex> m_TextureResourceIndices;
			std::vector<TextureResource>                          m_TextureResources;
		};
	}
}