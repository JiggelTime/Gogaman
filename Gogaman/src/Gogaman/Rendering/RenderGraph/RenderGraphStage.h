#pragma once

#include "RenderGraphTypeAlias.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class StageBuilder;

		//TODO: Change to struct?
		class Stage
		{
		public:
			using SetupFunction   = std::function<void(StageBuilder &&)>;
			using ExecuteFunction = std::function<void()>;
		public:
			inline Stage(const StageIndex index, const SetupFunction &setupFunction, const ExecuteFunction &executeFunction)
				: m_Index(index), m_SetupFunction(setupFunction), m_ExecuteFunction(executeFunction)
			{}

			inline ~Stage() = default;

			inline void Setup(StageBuilder &&builder) { m_SetupFunction(std::move(builder)); }
			inline void Execute() { m_ExecuteFunction(); }

			inline constexpr StageIndex GetIndex() const { return m_Index; }
			
			inline constexpr const std::vector<VirtualTextureIndex> &GetCreateTextures() const { return m_CreateTextures; }
			inline constexpr const std::vector<VirtualTextureIndex> &GetReadTextures()   const { return m_ReadTextures; }
			inline constexpr const std::vector<VirtualTextureIndex> &GetWriteTextures()  const { return m_WriteTextures; }
			inline void AddDestroyTexture(const VirtualTextureIndex index) { m_DestroyTextures.emplace_back(index); }
			inline constexpr const std::vector<VirtualTextureIndex> &GetDestroyTextures() const { return m_DestroyTextures; }

			inline constexpr const std::vector<VirtualRenderSurfaceIndex> &GetCreateRenderSurfaces() const { return m_CreateRenderSurfaces; }
			inline constexpr const std::vector<VirtualRenderSurfaceIndex> &GetReadRenderSurfaces()   const { return m_ReadRenderSurfaces; }
			inline void AddDestroyRenderSurface(const VirtualRenderSurfaceIndex index) { m_DestroyRenderSurfaces.emplace_back(index); }
			inline constexpr const std::vector<VirtualRenderSurfaceIndex> &GetDestroyRenderSurfaces() const { return m_DestroyRenderSurfaces; }
		private:
			friend StageBuilder;

			StageIndex m_Index;

			SetupFunction   m_SetupFunction;
			ExecuteFunction m_ExecuteFunction;

			std::vector<VirtualTextureIndex> m_CreateTextures;
			std::vector<VirtualTextureIndex> m_ReadTextures;
			std::vector<VirtualTextureIndex> m_WriteTextures;
			std::vector<VirtualTextureIndex> m_DestroyTextures;

			std::vector<VirtualRenderSurfaceIndex> m_CreateRenderSurfaces;
			std::vector<VirtualRenderSurfaceIndex> m_ReadRenderSurfaces;
			std::vector<VirtualRenderSurfaceIndex> m_DestroyRenderSurfaces;
		};
	}
}