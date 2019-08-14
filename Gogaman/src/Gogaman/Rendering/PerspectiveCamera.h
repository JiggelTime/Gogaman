#pragma once

#include "Gogaman/Geometry/RectangularFrustum.h"

#include <glm.hpp>

namespace Gogaman
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(const glm::vec3 &position = glm::vec3(0.0f), const glm::vec4 &rotation = glm::vec4(0.0f), const float focalLength = 50.0f, const float aperture = 5.6f, const float sensitivity = 100.0f, const float shutterSpeed = 1.0f / 100.0f, const float nearPlane = 0.1f, const float farPlane = 100.0f);
		~PerspectiveCamera();

		void Update();

		void SetPosition(const glm::vec3 &position);
		inline const glm::vec3 &GetPosition() const { return m_Position; }

		void SetRotation(const glm::vec4 &rotation);
		inline const glm::vec4 &GetRotation() const { return m_Rotation; }

		void SetFocalLength(const float focalLength);
		inline float GetFocalLength() const { return m_FocalLength; }

		void SetAperture(const float aperture);
		inline float GetAperture() const { return m_Aperture; }

		void SetSensitivity(const float sensitivity);
		inline float GetSensitivity() const { return m_Sensitivity; }

		void SetShutterSpeed(const float shutterSpeed);
		inline float GetShutterSpeed() const { return m_ShutterSpeed; }

		inline float GetExposure() const { return m_Exposure; }

		inline float GetBloomLuminanceCoeffecient() const { return m_BloomLuminanceCoeffecient; }

		inline void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
		inline float GetNearPlane() const { return m_NearPlane; }

		inline void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }
		inline float GetFarPlane() const { return m_FarPlane; }

		inline void SetFrustum(const RectangularFrustum &frustum) { m_Frustum = frustum; }
		inline const RectangularFrustum &GetFrustum() const { return m_Frustum; }

		inline const glm::mat4 &GetViewMatrix()                   const { return m_ViewMatrix;                   }
		inline const glm::mat4 &GetProjectionMatrix()             const { return m_ProjectionMatrix;             }
		inline const glm::mat4 &GetViewProjectionMatrix()         const { return m_ViewProjectionMatrix;         }
		inline const glm::mat4 &GetPreviousViewProjectionMatrix() const { return m_PreviousViewProjectionMatrix; }
	private:
		void UpdateExposure();
		void UpdateMatrices();
	private:
		glm::vec3          m_Position;
		glm::vec4          m_Rotation;

		//Unit: millimeters
		float              m_FocalLength;
		//Unit: f-stops
		float              m_Aperture;
		//Unit: saturation-based ISO sensitivity
		float              m_Sensitivity;
		//Unit: seconds
		float              m_ShutterSpeed;

		bool               m_ExposureRequiresUpdate;
		float              m_Exposure;
		float              m_BloomLuminanceCoeffecient;

		float              m_NearPlane, m_FarPlane;
		RectangularFrustum m_Frustum;

		bool               m_MatricesRequireUpdate;
		glm::mat4          m_ViewMatrix;
		glm::mat4          m_ProjectionMatrix;
		glm::mat4          m_ViewProjectionMatrix;
		glm::mat4          m_PreviousViewProjectionMatrix;
	};
}