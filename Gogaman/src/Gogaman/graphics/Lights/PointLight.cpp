#include "pch.h"
#include "PointLight.h"

namespace Gogaman
{
	uint PointLight::m_TotalPointLights = 0;

	PointLight::PointLight(const glm::vec3 &position, const glm::vec3 &color, const float coneAperture)
		: m_Position(position), m_Color(color), m_ConeAperture(coneAperture)
	{
		m_TotalPointLights++;
	}

	PointLight::~PointLight()
	{
		m_TotalPointLights--;
	}
}