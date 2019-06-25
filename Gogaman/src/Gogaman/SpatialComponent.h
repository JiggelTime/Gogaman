#pragma once

#include "Gogaman/ECS/Component.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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