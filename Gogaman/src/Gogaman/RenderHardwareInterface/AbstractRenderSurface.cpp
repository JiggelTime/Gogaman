#include "pch.h"
#include "AbstractRenderSurface.h"

#include "RenderSurface.h"

#include "Device.h"

namespace Gogaman
{
	namespace RHI
	{
		AbstractRenderSurface<RenderSurface>::AbstractRenderSurface(const uint8_t colorAttachmentCount, Attachment *colorAttachments, Attachment &&depthStencilAttachment, const uint16_t width, const uint16_t height, const uint16_t depth)
			: m_ColorAttachmentCount(colorAttachmentCount), m_ColorAttachments(colorAttachments), m_DepthStencilAttachment(std::move(depthStencilAttachment)), m_Width(width), m_Height(height), m_Depth(depth)
		{
			#if GM_RHI_DEBUGGING_ENABLED				
				GM_DEBUG_ASSERT(m_ColorAttachmentCount > 0,                                                     "Failed to construct render surface | Invalid color attachments | Color attachment count is 0");
				GM_DEBUG_ASSERT(m_ColorAttachmentCount < g_Device->GetRenderSurfaceColorAttachmentCountLimit(), "Failed to construct render surface | Invalid color attachments | Color attachment count exceeds device limit");
				for(uint8_t i = 0; i < m_ColorAttachmentCount; i++)
				{
					GM_DEBUG_ASSERT(GM_IS_VALID_ID(m_ColorAttachments[i].textureID), "Failed to construct render surface | Invalid color attachment | Invalid attachment texture ID");

					const Texture &texture = g_Device->GetResources().textures.Get(m_ColorAttachments[i].textureID);

					GM_DEBUG_ASSERT(texture.GetWidth()  >= m_Width,  "Failed to construct render surface | Invalid color attachment | Attachment texture width is less than render surface width");
					GM_DEBUG_ASSERT(texture.GetHeight() >= m_Height, "Failed to construct render surface | Invalid color attachment | Attachment texture height is less than render surface height");
					GM_DEBUG_ASSERT(texture.GetDepth()  >= m_Depth,  "Failed to construct render surface | Invalid color attachment | Attachment texture depth is less than render surface depth");
				}

				if(GM_IS_VALID_ID(m_DepthStencilAttachment.textureID))
				{
					const Texture &texture = g_Device->GetResources().textures.Get(m_DepthStencilAttachment.textureID);

					GM_DEBUG_ASSERT(texture.GetWidth()  >= m_Width,  "Failed to construct render surface | Invalid depth stencil attachment| Attachment texture width is less than render surface width");
					GM_DEBUG_ASSERT(texture.GetHeight() >= m_Height, "Failed to construct render surface | Invalid depth stencil attachment| Attachment texture height is less than render surface height");
					GM_DEBUG_ASSERT(texture.GetDepth()  >= m_Depth,  "Failed to construct render surface | Invalid depth stencil attachment| Attachment texture depth is less than render surface depth");
				}

				GM_DEBUG_ASSERT(m_Width  < g_Device->GetRenderSurfaceWidthLimit(),  "Failed to construct render surface | Width exceeds device limit");
				GM_DEBUG_ASSERT(m_Height < g_Device->GetRenderSurfaceHeightLimit(), "Failed to construct render surface | Height exceeds device limit");
				GM_DEBUG_ASSERT(m_Depth  < g_Device->GetRenderSurfaceDepthLimit(),  "Failed to construct render surface | Depth exceeds device limit");
			#endif
		}
	}
}