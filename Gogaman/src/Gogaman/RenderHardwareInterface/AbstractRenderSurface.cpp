#include "pch.h"
#include "AbstractRenderSurface.h"

#include "RenderSurface.h"

#include "Device.h"
#include "Gogaman/Core/Window.h"
#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		AbstractRenderSurface<RenderSurface>::AbstractRenderSurface(Attachments &&attachments, const uint16_t width, const uint16_t height, const uint16_t depth)
			: m_Attachments(std::move(attachments)), m_Width(width), m_Height(height), m_Depth(depth)
		{
			#if GM_RHI_DEBUGGING_ENABLED
				auto &device = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();
				
				GM_DEBUG_ASSERT(m_Attachments.colorAttachments.size() > 0,                                                  "Failed to construct render surface | Invalid attachments | Color attachment count is 0");
				GM_DEBUG_ASSERT(m_Attachments.colorAttachments.size() < device.GetRenderSurfaceColorAttachmentCountLimit(), "Failed to construct render surface | Invalid attachments | Color attachment count exceeds device limit");
				for(const Attachment &i : m_Attachments.colorAttachments)
				{
					GM_DEBUG_ASSERT(i.textureID.index != 0, "Failed to construct render surface | Invalid attachments | Invalid color attachment texture ID");
				}

				GM_DEBUG_ASSERT(m_Width  < device.GetRenderSurfaceWidthLimit(),  "Failed to construct render surface | Width exceeds device limit");
				GM_DEBUG_ASSERT(m_Height < device.GetRenderSurfaceHeightLimit(), "Failed to construct render surface | Height exceeds device limit");
				GM_DEBUG_ASSERT(m_Depth  < device.GetRenderSurfaceDepthLimit(),  "Failed to construct render surface | Depth exceeds device limit");
			#endif
		}
	}
}