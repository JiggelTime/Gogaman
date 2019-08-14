#pragma once

#include <glm.hpp>

namespace Gogaman
{
	class BoundingBox3D
	{
	public:
		inline BoundingBox3D(const glm::vec3 &minimum = glm::vec3(std::numeric_limits<float>::lowest()), const glm::vec3 &maximum = glm::vec3(std::numeric_limits<float>::max()))
			: m_Minimum(minimum), m_Maximum(maximum)
		{}

		inline BoundingBox3D(const glm::vec3 &point)
			: m_Minimum(point), m_Maximum(point)
		{}

		inline ~BoundingBox3D()
		{}

		inline bool operator==(const BoundingBox3D &other) const { return m_Minimum == other.m_Minimum && m_Maximum == other.m_Maximum; }
		inline bool operator!=(const BoundingBox3D &other) const { return m_Minimum != other.m_Minimum && m_Maximum != other.m_Maximum; }

		inline const glm::vec3 &Diagonal() const { return m_Maximum - m_Minimum; }

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

		inline bool Contains(const glm::vec3 &point) const { return (m_Minimum.x <= point.x) && (m_Minimum.y <= point.y) && (m_Minimum.z <= point.z) && (m_Maximum.x >= point.x) && (m_Maximum.y >= point.y) && (m_Maximum.z >= point.z); }
	private:
		glm::vec3 m_Minimum;
		glm::vec3 m_Maximum;
	};
}