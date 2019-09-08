#pragma once

#include "Gogaman/ECS/Component.h"

#include <glm.hpp>

namespace Gogaman
{
	struct SpatialComponent : public Component<SpatialComponent>
	{
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		float     rotationAngle;
	};
}