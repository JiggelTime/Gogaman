#pragma once

#include "Gogaman/Core/ResourceContainer.h"

#include "RenderSurface.h"

namespace Gogaman
{
	namespace RHI
	{
		using RenderSurfaceContainer = ResourceContainer<RenderSurface, uint8_t, 64>;
		using RenderSurfaceID        = RenderSurfaceContainer::ID;
	}
}