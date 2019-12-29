#pragma once

#include "Gogaman/RenderHardwareInterface/Identifier.h"

namespace Gogaman
{
	struct PBR_Material
	{
		RHI::TextureID albedo;
		RHI::TextureID normal;
		RHI::TextureID roughness;
		RHI::TextureID metalness;
		RHI::TextureID emissivity;
	};
}