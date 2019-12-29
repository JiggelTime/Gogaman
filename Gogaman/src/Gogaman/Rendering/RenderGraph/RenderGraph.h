#pragma once

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include "RenderGraphTypeAlias.h"
#include "RenderGraphStage.h"
#include "RenderGraphResource.h"

#include "Gogaman/Rendering/RenderingContext.h"

#include "Gogaman/Application.h"

#include "Gogaman/Rendering/RenderSurface.h"
#include "Gogaman/Rendering/Texture/Texture2D.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		//Stripped down version of render stage for use at runtime
		struct RenderOperation
		{

		};

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

			inline Texture2D &GetTexture(const std::string &name) { return GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_VirtualTexturePhysicalIDs[GetVirtualTexture(name).GetID()]); }
			
			inline RenderSurface &GetRenderSurface(const std::string &name) { return GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_VirtualRenderSurfacePhysicalIDs[GetVirtualRenderSurface(name).GetID()]); }
		private:
			void GenerateDependencies(const Stage &stage);

			VirtualTexture &GetVirtualTexture(const std::string &name);

			VirtualRenderSurface &GetVirtualRenderSurface(const std::string &name);
		private:
			friend StageBuilder;

			//std::vector<RenderOperation>         m_RenderSchedule;
			std::vector<StageIndex>              m_StageSchedule;
			std::vector<std::vector<StageIndex>> m_StageDependencies;
			std::vector<Stage>                   m_Stages;
			std::string                          m_BackBuffer;

			std::unordered_map<std::string, VirtualTextureIndex> m_VirtualTextureIndices;
			std::unordered_map<VirtualTextureID, Texture2D_ID>   m_VirtualTexturePhysicalIDs;
			std::vector<VirtualTexture>                          m_VirtualTextures;

			std::unordered_map<std::string, VirtualRenderSurfaceIndex>  m_VirtualRenderSurfaceIndices;
			std::unordered_map<VirtualRenderSurfaceID, RenderSurfaceID> m_VirtualRenderSurfacePhysicalIDs;
			std::vector<VirtualRenderSurface>                           m_VirtualRenderSurfaces;
		};
	}
}