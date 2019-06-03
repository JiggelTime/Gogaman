#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace gogaman
{
	namespace maths
	{
		inline float toRadians(float degrees)
		{
			return (M_PI / 180.0f) * degrees;
		}
	}
}