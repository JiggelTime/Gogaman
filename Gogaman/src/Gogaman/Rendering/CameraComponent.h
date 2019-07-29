#pragma once

#include "Gogaman/ECS/Component.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	struct CameraComponent : public Component<CameraComponent>
	{
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
	};
}