#pragma once

#include "RenderGraphTypeAlias.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class TextureResource;
		class StageBuilder;

		//TODO: Change to a struct?
		class Stage
		{
		public:
			//using SetupFunction   = std::function<void(StageBuilder &)>;
			//using ExecuteFunction = std::function<void()>;
		public:
			inline Stage(const StageIndex index)
				: m_Index(index)
			{}

			inline ~Stage() = default;

			//inline void Setup(StageBuilder &builder) { m_SetupFunction(builder); }
			//inline void Execute() { m_ExecuteFunction(); }

			inline constexpr StageIndex GetIndex() const { return m_Index; }
			
			inline constexpr const std::vector<TextureResourceIndex> &GetCreateTextureResources() const { return m_CreateTextureResources; }
			inline constexpr const std::vector<TextureResourceIndex> &GetReadTextureResources() const { return m_ReadTextureResources; }
			inline constexpr const std::vector<TextureResourceIndex> &GetWriteTextureResources() const { return m_WriteTextureResources; }

			inline void AddDestroyTextureResource(const TextureResourceIndex index) { m_DestroyTextureResources.emplace_back(index); }
			inline constexpr const std::vector<TextureResourceIndex> &GetDestroyTextureResources() const { return m_DestroyTextureResources; }
		private:
			friend StageBuilder;

			StageIndex m_Index;

			//SetupFunction   m_SetupFunction;
			//ExecuteFunction m_ExecuteFunction;

			std::vector<TextureResourceIndex> m_CreateTextureResources;
			std::vector<TextureResourceIndex> m_ReadTextureResources;
			std::vector<TextureResourceIndex> m_WriteTextureResources;
			std::vector<TextureResourceIndex> m_DestroyTextureResources;
		};
	}
}