#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphStage.h"
#include "RenderGraph.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		//class Stage;
		//class TextureResource;
		//class Graph;
		
		class StageBuilder
		{
		public:
			inline StageBuilder(Graph *graph, Stage *stage)
				: m_Graph(graph), m_Stage(stage)
			{}

			inline ~StageBuilder() = default;

			template<const char *name>
			VirtualTexture &CreateTexture(TextureDescriptor &&descriptor)
			{
				VirtualTexture &textureResource = m_Graph->m_VirtualTextures.emplace_back(VirtualTexture(name, std::move(descriptor)));
				m_Graph->m_VirtualTextureIndices[name] = textureResource.GetIndex();

				textureResource.AddWriteStage(m_Stage->GetIndex());
				m_Stage->m_CreateTextures.emplace_back(textureResource.GetIndex());
				m_Stage->m_WriteTextures.emplace_back(textureResource.GetIndex());
				return textureResource;
			}

			template<const char *name>
			VirtualTexture &ReadTexture()
			{
				VirtualTexture &textureResource = m_Graph->GetVirtualTexture(name);
				textureResource.AddReadStage(m_Stage->GetIndex());
				m_Stage->m_ReadTextures.emplace_back(textureResource.GetIndex());
				return textureResource;
			}

			template<const char *name, const char *outputName>
			VirtualTexture &WriteTexture()
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
		private:
			Graph *m_Graph;
			Stage *m_Stage;
		};
	}
}