#include "pch.h"
#include "PointLight.h"

namespace Gogaman
{
	int PointLight::m_NumPointLights = 0;

	PointLight::PointLight(const glm::vec3 &position, const glm::vec3 &color, const float coneAperture)
		: m_Position(position), m_Color(color), m_ConeAperture(coneAperture)
	{
		m_NumPointLights++;
	}

	PointLight::~PointLight()
	{
		m_NumPointLights--;
	}
}