#pragma once

#include "RenderGraphTypeAlias.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		class Stage;
		
		//TODO: make Resource a parent class?
		class TextureResource
		{
		public:
			inline TextureResource(const std::string &name)
				: m_Name(name)
			{
				static TextureResourceIndex nextIndex = 0;
				m_Index = nextIndex++;
			}

			inline ~TextureResource() = default;

			inline constexpr const std::string &GetName() const { return m_Name; }

			inline constexpr TextureResourceIndex GetIndex() const { return m_Index; }

			inline constexpr TextureResourcePhysicalID GetPhysicalID() const { return m_PhysicalID; }
			inline void SetPhysicalID(const TextureResourcePhysicalID identifier) { m_PhysicalID = identifier; }

			inline void AddReadStage(const StageIndex index) { m_ReadStages.emplace_back(index); }
			inline constexpr const std::vector<StageIndex> &GetReadStages() const { return m_ReadStages; }

			inline void AddWriteStage(const StageIndex index) { m_WriteStages.emplace_back(index); }
			inline constexpr const std::vector<StageIndex> &GetWriteStages() const { return m_WriteStages; }
		private:
			std::string               m_Name;
			TextureResourceIndex      m_Index;
			TextureResourcePhysicalID m_PhysicalID;
			std::vector<StageIndex>   m_ReadStages;
			std::vector<StageIndex>   m_WriteStages;
		};
	}
}