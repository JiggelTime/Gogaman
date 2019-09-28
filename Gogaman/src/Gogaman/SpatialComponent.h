#pragma once

#include <glm.hpp>

namespace Gogaman
{
	struct SpatialComponent
	{
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		float     rotationAngle;
	};
}