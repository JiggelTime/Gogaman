#pragma once

#include "Gogaman/RenderHardwareInterface/DeviceMemory.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Buffer.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		struct Texture
		{
			uint16_t             width      = 1;
			uint16_t             height     = 1;
			uint16_t             depth      = 1;
			uint8_t              levelCount = 1;
			RHI::Texture::Format format;
		};

		struct Buffer
		{
			uint32_t                size;
			RHI::DeviceMemory::Type memoryType;
			RHI::Buffer::BindFlag   bindFlags = RHI::Buffer::BindFlag::None;
		};
	}
}