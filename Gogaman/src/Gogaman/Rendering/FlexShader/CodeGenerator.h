#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/FlexShader/VulkanCodeGenerator.h"
#else
	#error "Unsupported platform"
#endif