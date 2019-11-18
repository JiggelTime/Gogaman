#include "pch.h"
#include "AbstractTexture.h"

#include "Device.h"
#include "Gogaman/Core/Window.h"
#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		AbstractTexture<ImplementationType>::AbstractTexture(const uint16_t width, const uint16_t height, const uint16_t depth, const uint8_t levelCount)
			: m_Width(width), m_Height(height), m_Depth(depth), m_LevelCount(levelCount)
		{
			#if GM_RHI_DEBUGGING_ENABLED
				const Device &device = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();

				GM_ASSERT(m_Width  < device.GetTextureWidthLimit(),  "Failed to construct texture | Width exceeds device limit");
				GM_ASSERT(m_Height < device.GetTextureHeightLimit(), "Failed to construct texture | Height exceeds device limit");
				GM_ASSERT(m_Depth  < device.GetTextureDepthLimit(),  "Failed to construct texture | Depth exceeds device limit");
			#endif
		}
	}
}