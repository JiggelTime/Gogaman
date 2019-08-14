#pragma once

#include <glm.hpp>

namespace Gogaman
{
	struct Ray3D
	{
		inline Ray3D()
		{}

		glm::vec3 position;
		glm::vec3 direction;
	};
}