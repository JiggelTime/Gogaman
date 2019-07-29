#pragma once

#include "Gogaman/ECS/Component.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct PointLightComponent : public Component<PointLightComponent>
	{
		glm::vec3 position;
		glm::vec3 radiance;
	};

	struct DirectionalLightComponent : public Component<DirectionalLightComponent>
	{
		glm::vec3 direction;
		glm::vec3 radiance;
	};
}