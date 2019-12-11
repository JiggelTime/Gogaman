#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanCommandBuffer.h"
#else
	#error "Unsupported platform"
#endif