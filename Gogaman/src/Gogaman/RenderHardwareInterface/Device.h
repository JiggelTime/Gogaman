#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Vulkan/RenderHardwareInterface/VulkanDevice.h"

	namespace Gogaman { namespace RHI { using Device = VulkanDevice; } }
#else
	#error
#endif