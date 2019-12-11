#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanDescriptorGroupLayout.h"
#else
	#error "Unsupported platform"
#endif