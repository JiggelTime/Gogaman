#pragma once

#include <glm.hpp>

namespace Gogaman
{
	struct BoundingBox3D
	{
	public:
		inline BoundingBox3D(const glm::vec3 &minimum = glm::vec3(std::numeric_limits<float>::lowest()), const glm::vec3 &maximum = glm::vec3(std::numeric_limits<float>::max()))
			: minimum(minimum), maximum(maximum)
		{}

		inline BoundingBox3D(const glm::vec3 &point)
			: minimum(point), maximum(point)
		{}

		inline bool operator==(const BoundingBox3D &other) const { return minimum == other.minimum && maximum == other.maximum; }
		inline bool operator!=(const BoundingBox3D &other) const { return minimum != other.minimum && maximum != other.maximum; }

		inline const glm::vec3 &Diagonal() const { return maximum - minimum; }

		inline float SurfaceArea() const
		{
			glm::vec3 diagonal = Diagonal();
			return 2.0f * (diagonal.x * diagonal.y + diagonal.x * diagonal.z + diagonal.y * diagonal.z);
		}

		inline float Volume() const
		{
			glm::vec3 diagonal = Diagonal();
			return diagonal.x * diagonal.y * diagonal.z;
		}

		inline bool Contains(const glm::vec3 &point) const { return (minimum.x <= point.x) && (minimum.y <= point.y) && (minimum.z <= point.z) && (maximum.x >= point.x) && (maximum.y >= point.y) && (maximum.z >= point.z); }

		glm::vec3 minimum;
		glm::vec3 maximum;
	};
}