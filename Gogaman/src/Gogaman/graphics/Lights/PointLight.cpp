#include "pch.h"
#include "PointLight.h"

namespace Gogaman
{
	unsigned int PointLight::numLights = 0;

	PointLight::PointLight(const glm::vec3 &position, const glm::vec3 &color, const float &coneAperture)
		: position(position), color(color), coneAperture(coneAperture)
	{
		numLights++;
	}
}