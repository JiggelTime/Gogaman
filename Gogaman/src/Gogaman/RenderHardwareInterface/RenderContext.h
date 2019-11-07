#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanRenderContext.h"

	namespace Gogaman { namespace RHI { using RenderContext = VulkanRenderContext; } }
#else
	#error
#endif