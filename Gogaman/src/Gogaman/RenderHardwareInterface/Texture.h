#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanTexture.h"
#else
	#error "Unsupported platform"
#endif