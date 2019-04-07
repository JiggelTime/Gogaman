#pragma once

#include <glm.hpp>

struct PointLight
{
	PointLight(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(1.0f), const float &coneAperture = 0.001f)
		: position(position), color(color), coneAperture(coneAperture)
	{ }
	
	glm::vec3 position;
	glm::vec3 color;
	float     coneAperture;
};