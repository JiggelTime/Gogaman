#include "pch.h"
#include "RenderGraphStageBuilder.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		VirtualTexture &StageBuilder::CreateTexture(const std::string &name, TextureDescriptor &&descriptor)
		{
			VirtualTexture &textureResource = m_Graph->m_VirtualTextures.emplace_back(VirtualTexture(name, std::move(descriptor)));
			m_Graph->m_VirtualTextureIndices[name] = textureResource.GetIndex();

			textureResource.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_CreateTextures.emplace_back(textureResource.GetIndex());
			m_Stage->m_WriteTextures.emplace_back(textureResource.GetIndex());
			return textureResource;
		}

		VirtualTexture &StageBuilder::ReadTexture(const std::string &name)
		{
			VirtualTexture &textureResource = m_Graph->GetVirtualTexture(name);
			textureResource.AddReadStage(m_Stage->GetIndex());
			m_Stage->m_ReadTextures.emplace_back(textureResource.GetIndex());
			return textureResource;
		}

		VirtualTexture &StageBuilder::WriteTexture(const std::string &name, const std::string &outputName)
		{
			GM_ASSERT(outputName != name, "Failed to write texture resource | Invalid output name")

				VirtualTexture &input = ReadTexture<name>();

			TextureDescriptor inputDescriptorCopy = input.GetDescriptor();
			VirtualTexture &output = m_Graph->m_VirtualTextures.emplace_back(VirtualTexture(outputName, std::move(inputDescriptorCopy)));
			m_Graph->m_VirtualTextureIndices[outputName] = output.GetIndex();

			output.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_WriteTextures.emplace_back(output.GetIndex());

			output.SetID(input.GetID());
			return output;
		}
	}
}