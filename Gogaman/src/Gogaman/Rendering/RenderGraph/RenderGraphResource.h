#pragma once

#include "RenderGraphTypeAlias.h"

#include "Gogaman/Rendering/RenderingContext.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		template<typename ResourceIndexType, typename ResourceIdentifierType, typename ResourceDescriptorType>
		class VirtualResource
		{
		public:
			inline VirtualResource(const std::string &name, ResourceDescriptorType &&descriptor)
				: m_Name(name), m_Descriptor(descriptor)
			{
				static ResourceIndexType nextIndex = 0;
				m_Index = nextIndex++;

				static ResourceIdentifierType nextID = 0;
				m_ID = nextID++;
			}

			inline ~VirtualResource() = default;

			inline constexpr const std::string &GetName() const { return m_Name; }

			inline constexpr ResourceIndexType GetIndex() const { return m_Index; }

			inline constexpr ResourceIdentifierType GetID() const { return m_ID; }
			inline void SetID(const ResourceIdentifierType identifier) { m_ID = identifier; }

			inline void AddReadStage(const StageIndex index) { m_ReadStages.emplace_back(index); }
			inline constexpr const std::vector<StageIndex> &GetReadStages() const { return m_ReadStages; }

			inline void AddWriteStage(const StageIndex index) { m_WriteStages.emplace_back(index); }
			inline constexpr const std::vector<StageIndex> &GetWriteStages() const { return m_WriteStages; }

			inline constexpr const ResourceDescriptorType &GetDescriptor() const { return m_Descriptor; }
		private:
			std::string             m_Name;
			ResourceIndexType       m_Index;
			ResourceIdentifierType  m_ID;
			std::vector<StageIndex> m_ReadStages;
			std::vector<StageIndex> m_WriteStages;
			ResourceDescriptorType m_Descriptor;
		};

		struct TextureDescriptor
		{
			uint16_t              width, height;
			TextureInternalFormat internalFormat;
			TextureFormat         format;
			TextureInterpolation  interpolation;
			uint8_t               levels;
		};

		using VirtualTexture = VirtualResource<VirtualTextureIndex, VirtualTextureID, TextureDescriptor>;
	}
}