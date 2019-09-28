#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct PointLightComponent
	{
		glm::vec3 position;
		glm::vec3 radiance;
	};

	struct DirectionalLightComponent
	{
		glm::vec3 direction;
		glm::vec3 radiance;
	};
}