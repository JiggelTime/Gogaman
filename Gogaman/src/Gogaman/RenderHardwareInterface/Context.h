#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanContext.h"
#else
	#error "Unsupported platform"
#endif