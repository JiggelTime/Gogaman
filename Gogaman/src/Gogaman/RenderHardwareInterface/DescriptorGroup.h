#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanDescriptorGroup.h"
#else
	#error "Unsupported platform"
#endif