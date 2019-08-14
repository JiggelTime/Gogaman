#pragma once

#include "Plane.h"

#include <glm.hpp>

namespace Gogaman
{
	struct RectangularFrustum
	{
		inline RectangularFrustum()
		{}

		Plane planes[6];
	};
}