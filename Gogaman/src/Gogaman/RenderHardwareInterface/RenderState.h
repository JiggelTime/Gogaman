#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanRenderState.h"
#else
	#error "Unsupported platform"
#endif