#pragma once

#include "Sphere.h"

#include <glm.hpp>

namespace Gogaman
{
	struct Plane
	{
		inline Plane(const glm::vec3 &normal = glm::vec3(0.0f, 1.0f, 0.0f), const float distance = 0.0f)
			: normal(normal), distance(distance)
		{}

		inline float Distance(const Sphere &sphere) const
		{

		}

		inline bool Intersects(const Plane &plane) const
		{
			glm::vec3 intersectionDirection = glm::cross(normal, plane.normal);
			return intersectionDirection.length() != 0;
		}

		inline bool Intersects(const Sphere &sphere) const
		{
			float     distance     = glm::dot(normal, sphere.position) - distance;
			glm::vec3 closestPoint = sphere.position - normal * distance;
			return distance < sphere.radius;
		}

		glm::vec3 normal;
		float     distance;
	};
}