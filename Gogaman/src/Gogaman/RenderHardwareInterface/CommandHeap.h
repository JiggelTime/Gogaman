#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanCommandHeap.h"
#else
	#error "Unsupported platform"
#endif