#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanTransferCommandRecorder.h"
#else
	#error "Unsupported platform"
#endif