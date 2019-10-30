#include "pch.h"
#include "RenderGraphStageBuilder.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include "RenderGraphStage.h"
#include "RenderGraphResource.h"
#include "RenderGraph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		TextureResource &StageBuilder::CreateTextureResource(const std::string &name)
		{
			TextureResource &textureResource = m_Graph->m_TextureResources.emplace_back(TextureResource(name));
			m_Graph->m_TextureResourceIndices[name] = textureResource.GetIndex();

			textureResource.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_CreateTextureResources.emplace_back(textureResource.GetIndex());
			m_Stage->m_WriteTextureResources.emplace_back(textureResource.GetIndex());
			return textureResource;
		}

		TextureResource &StageBuilder::ReadTextureResource(const std::string &name)
		{
			TextureResource &textureResource = m_Graph->GetTextureResource(name);
			textureResource.AddReadStage(m_Stage->GetIndex());
			m_Stage->m_ReadTextureResources.emplace_back(textureResource.GetIndex());
			return textureResource;
		}

		TextureResource &StageBuilder::WriteTextureResource(const std::string &name, const std::string &outputName)
		{
			GM_ASSERT(outputName != name, "Failed to write texture resource | Invalid output name")

			TextureResourcePhysicalID physicalID = ReadTextureResource(name).GetPhysicalID();

			TextureResource &output = CreateTextureResource(outputName);
			output.SetPhysicalID(physicalID);
			return output;
		}
	}
}