#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanShader.h"
#else
	#error "Unsupported platform"
#endif