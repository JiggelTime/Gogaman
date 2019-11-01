#pragma once

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include "RenderGraphTypeAlias.h"
#include "RenderGraphStage.h"
#include "RenderGraphResource.h"

#include "Gogaman/Rendering/RenderingContext.h"

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

			void CreateStage(const Stage::SetupFunction &setupFunction, const Stage::ExecuteFunction &executeFunction);
			
			void SetBackBuffer(const std::string &name);

			void Compile();

			void Execute();

			void Log() const;
		private:
			void GenerateDependencies(const Stage &stage);

			VirtualTexture &GetVirtualTexture(const std::string &name);
		private:
			friend StageBuilder;

			std::vector<std::vector<StageIndex>>                  m_StageDependencies;
			std::vector<StageIndex>                               m_StageSchedule;
			std::vector<Stage>                                    m_Stages;
			std::string                                           m_BackBuffer;
			std::unordered_map<std::string, VirtualTextureIndex> m_VirtualTextureIndices;
			std::unordered_map<VirtualTextureID, Texture2D_ID>   m_VirtualTexturePhysicalIDs;
			std::vector<VirtualTexture>                          m_VirtualTextures;
		};
	}
}