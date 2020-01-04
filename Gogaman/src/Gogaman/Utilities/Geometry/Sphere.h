#pragma once

#include <glm.hpp>

namespace Gogaman
{
	struct Sphere
	{
		inline Sphere(const glm::vec3 &position = glm::vec3(0.0f), const float radius = 1.0f)
			: position(position), radius(radius)
		{}

		glm::vec3 position;
		float     radius;
	};
}