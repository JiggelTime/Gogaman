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
				GM_DEBUG_ASSERT(m_ColorAttachmentCount > 0,                                                     "Failed to construct render surface | Invalid attachments | Color attachment count is 0");
				GM_DEBUG_ASSERT(m_ColorAttachmentCount < g_Device->GetRenderSurfaceColorAttachmentCountLimit(), "Failed to construct render surface | Invalid attachments | Color attachment count exceeds device limit");
				for(uint8_t i = 0; i < m_ColorAttachmentCount; i++)
				{
					GM_DEBUG_ASSERT(m_ColorAttachments[i].textureID.index != GM_INVALID_ID_INDEX, "Failed to construct render surface | Invalid attachments | Invalid color attachment texture ID");
				}

				GM_DEBUG_ASSERT(m_Width  < g_Device->GetRenderSurfaceWidthLimit(),  "Failed to construct render surface | Width exceeds device limit");
				GM_DEBUG_ASSERT(m_Height < g_Device->GetRenderSurfaceHeightLimit(), "Failed to construct render surface | Height exceeds device limit");
				GM_DEBUG_ASSERT(m_Depth  < g_Device->GetRenderSurfaceDepthLimit(),  "Failed to construct render surface | Depth exceeds device limit");
			#endif
		}
	}
}