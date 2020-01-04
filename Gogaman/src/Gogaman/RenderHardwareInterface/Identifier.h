#pragma once

#include "Gogaman/Utilities/Container/ResourceContainer.h"

namespace Gogaman
{
	namespace RHI
	{
		using SamplerID       = ResourceID<uint8_t>;
		using TextureID       = ResourceID<uint16_t>;
		using BufferID        = ResourceID<uint16_t>;
		using RenderSurfaceID = ResourceID<uint8_t>;
		using ShaderID        = ResourceID<uint8_t>;
		using ShaderProgramID = ResourceID<uint8_t>;
	}
}