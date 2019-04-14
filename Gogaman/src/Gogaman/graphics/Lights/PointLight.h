#pragma once

#include <glm.hpp>
#include "Gogaman/Core.h"

namespace Gogaman
{
	class GOGAMAN_API PointLight
	{
	public:
		PointLight(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(1.0f), const float &coneAperture = 0.001f);

		glm::vec3 position;
		glm::vec3 color;
		float coneAperture;

		static unsigned int numLights;
	};
}