#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanDevice.h"

	inline std::unique_ptr<Gogaman::RHI::Device> g_Device;
#else
	#error "Unsupported platform"
#endif