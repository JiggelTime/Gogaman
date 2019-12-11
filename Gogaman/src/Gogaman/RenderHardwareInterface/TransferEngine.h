#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanTransferEngine.h"
#else
	#error "Unsupported platform"
#endif