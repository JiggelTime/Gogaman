#include "pch.h"
#include "RenderGraphStageBuilder.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		VirtualTexture &StageBuilder::CreateTexture(const std::string &name, TextureDescriptor &&descriptor)
		{
			for(const VirtualTexture &i : m_Graph->m_VirtualTextures)
			{
				GM_ASSERT(name != i.GetName(), "Failed to create render graph texture | Texture with name \"%s\" already exists", name.c_str())
			}

			VirtualTexture &virtualTexture = m_Graph->m_VirtualTextures.emplace_back(VirtualTexture(name, std::move(descriptor)));
			m_Graph->m_VirtualTextureIndices[name] = virtualTexture.GetIndex();

			virtualTexture.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_CreateTextures.emplace_back(virtualTexture.GetIndex());
			m_Stage->m_WriteTextures.emplace_back(virtualTexture.GetIndex());
			return virtualTexture;
		}

		VirtualTexture &StageBuilder::ReadTexture(const std::string &name)
		{
			VirtualTexture &virtualTexture = m_Graph->GetVirtualTexture(name);
			virtualTexture.AddReadStage(m_Stage->GetIndex());
			m_Stage->m_ReadTextures.emplace_back(virtualTexture.GetIndex());
			return virtualTexture;
		}

		VirtualTexture &StageBuilder::WriteTexture(const std::string &name, const std::string &outputName)
		{
			GM_ASSERT(outputName != name, "Failed to write render graph texture | Output name \"%s\" is equal to input name", outputName.c_str())

			VirtualTexture &input = ReadTexture(name);

			TextureDescriptor inputDescriptorCopy = input.GetDescriptor();
			VirtualTexture &output = m_Graph->m_VirtualTextures.emplace_back(VirtualTexture(outputName, std::move(inputDescriptorCopy)));
			m_Graph->m_VirtualTextureIndices[outputName] = output.GetIndex();

			output.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_WriteTextures.emplace_back(output.GetIndex());

			output.SetID(input.GetID());
			return output;
		}

		VirtualRenderSurface &StageBuilder::CreateRenderSurface(const std::string &name, RenderSurfaceDescriptor &&descriptor)
		{
			for(const VirtualRenderSurface &i : m_Graph->m_VirtualRenderSurfaces)
			{
				GM_ASSERT(name != i.GetName(), "Failed to create render graph render surface | Render surface with name \"%s\" already exists", name.c_str())
			}

			VirtualRenderSurface &virtualRenderSurface = m_Graph->m_VirtualRenderSurfaces.emplace_back(VirtualRenderSurface(name, std::move(descriptor)));
			m_Graph->m_VirtualRenderSurfaceIndices[name] = virtualRenderSurface.GetIndex();

			//virtualRenderSurface.AddWriteStage(m_Stage->GetIndex());
			m_Stage->m_CreateRenderSurfaces.emplace_back(virtualRenderSurface.GetIndex());
			return virtualRenderSurface;
		}

		VirtualRenderSurface &StageBuilder::ReadRenderSurface(const std::string &name)
		{
			VirtualRenderSurface &virtualRenderSurface = m_Graph->GetVirtualRenderSurface(name);
			virtualRenderSurface.AddReadStage(m_Stage->GetIndex());
			m_Stage->m_ReadRenderSurfaces.emplace_back(virtualRenderSurface.GetIndex());
			return virtualRenderSurface;
		}
	}
}