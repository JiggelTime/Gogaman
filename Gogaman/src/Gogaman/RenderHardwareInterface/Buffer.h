#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanBuffer.h"
#else
	#error "Unsupported platform"
#endif