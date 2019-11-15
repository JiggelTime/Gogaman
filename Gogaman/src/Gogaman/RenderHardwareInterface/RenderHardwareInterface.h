#pragma once

#include "TextureResource.h"
#include "RenderSurfaceResource.h"

#include "Device.h"

namespace Gogaman
{
	using namespace RHI;

	struct RenderHardwareInterface
	{
		struct Resources
		{
			TextureContainer       textures;
			RenderSurfaceContainer renderSurfaces;
		};

		Device    device;
		Resources resources;
	};
}
