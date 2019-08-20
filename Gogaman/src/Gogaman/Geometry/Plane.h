#pragma once

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include "Sphere.h"
#include "RectangularFrustum.h"

#include <glm.hpp>

namespace Gogaman
{
	struct Plane
	{
		inline Plane()
		{}

		inline Plane(const glm::vec3 &normal, const float distance)
			: normal(normal), distance(distance)
		{}

		//Shortest signed distance from plane to point
		inline float Distance(const glm::vec3 &point) const
		{
;			GM_ASSERT(abs(glm::length(normal)) - 1.0f < 0.000001f, "Failed to compute distance to point | Plane equation is not normalized | Normal vector length: %f", glm::length(normal))
			return glm::dot(normal, point) + distance;
		}

		//Shortest signed distance from plane to sphere
		inline float Distance(const Sphere &sphere) const
		{
			return Distance(sphere.position) - sphere.radius;
		}

		inline bool Intersects(const glm::vec3 &point) const
		{
			return Distance(point) < 0.0f;
		}

		inline bool Intersects(const Plane &plane) const
		{
			glm::vec3 intersectionDirection = glm::cross(normal, plane.normal);
			return glm::length(intersectionDirection) != 0;
		}

		inline bool Intersects(const Sphere &sphere) const
		{
			return Distance(sphere) < 0.0f;
		}

		//ax + by + cz + d = 0
		glm::vec3 normal;
		float     distance;
	};
}