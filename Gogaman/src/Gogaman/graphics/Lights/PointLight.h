#pragma once

#include <glm.hpp>
#include "Gogaman/Base.h"

namespace Gogaman
{
	class PointLight
	{
	public:
		PointLight(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec3 &color = glm::vec3(1.0f), const float coneAperture = 0.001f);
		~PointLight();

		inline void SetPosition(const glm::vec3 &position) { m_Position = position; }
		inline glm::vec3 GetPosition() const { return m_Position; }

		inline void SetColor(const glm::vec3 &position) { m_Color = position; }
		inline glm::vec3 GetColor() const { return m_Color; }

		inline void SetConeAperture(const float coneAperture) { m_ConeAperture = coneAperture; }
		inline float GetConeAperture() const { return m_ConeAperture; }

		inline static uint GetTotalPointLights() { return m_TotalPointLights; }
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Color;
		float     m_ConeAperture;
		static uint m_TotalPointLights;
	};
}