#pragma once

#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Buffer.h"

namespace Gogaman
{
	namespace RenderGraph
	{
		struct Resource
		{
			std::string name;
			//Stage::Index edge[2];
			bool isPersistent = false;
		};

		struct Texture : public Resource
		{
			uint16_t             width, height, depth;
			uint8_t              levelCount = 1;
			RHI::Texture::Format format;
		};

		struct Buffer : public Resource
		{
			uint32_t              size;
			RHI::Buffer::BindFlag bindFlags = RHI::Buffer::BindFlag::None;
		};
	}
}