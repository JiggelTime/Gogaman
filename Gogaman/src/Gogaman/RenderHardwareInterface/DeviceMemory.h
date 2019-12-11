#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanDeviceMemory.h"
#else
	#error "Unsupported platform"
#endif