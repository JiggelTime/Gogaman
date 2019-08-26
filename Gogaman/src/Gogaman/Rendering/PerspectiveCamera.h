#pragma once

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Gogaman/Geometry/RectangularFrustum.h"

#include <glm.hpp>

namespace Gogaman
{
	//TODO: Relocate this, belongs in math
	struct EulerAngles
	{
		inline EulerAngles(const float pitch = 0.0f, const float yaw = 0.0f, const float roll = 0.0f)
			: pitch(pitch), yaw(yaw), roll(roll)
		{}

		float pitch;
		float yaw;
		float roll;
	};

	class PerspectiveCamera : public EventListener
	{
	public:
		PerspectiveCamera(const glm::vec3 &position = glm::vec3(0.0f), const EulerAngles &rotation = EulerAngles(), const float focalLength = 50.0f, const float SensorHeight = 8.8f, const float aspectRatio = 1.77777f, const float aperture = 5.6f, const float sensitivity = 100.0f, const float shutterSpeed = 1.0f / 100.0f, const float nearPlane = 0.1f, const float farPlane = 100.0f);
		~PerspectiveCamera();

		void Update();

		virtual void OnEvent(Event &event) override;

		void SetPosition(const glm::vec3 &position) { m_Position = position; }
		inline const glm::vec3 &GetPosition() const { return m_Position; }

		void SetRotation(const EulerAngles &rotation) { m_Rotation = rotation; }
		inline const EulerAngles &GetRotation() const { return m_Rotation; }

		void SetFocalLength(const float focalLength);
		inline float GetFocalLength() const { return m_FocalLength; }

		void SetSensorHeight(const float sensorHeight);
		inline float GetSensorHeight() const { return m_SensorHeight; }

		void SetAspectRatio(float aspectRatio);
		inline float GetAspectRatio() const { return m_AspectRatio; }

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

		inline const RectangularFrustum &GetFrustum() const { return m_Frustum; }

		inline const glm::mat4 &GetViewMatrix()                   const { return m_ViewMatrix;                   }
		inline const glm::mat4 &GetProjectionMatrix()             const { return m_ProjectionMatrix;             }
		inline const glm::mat4 &GetViewProjectionMatrix()         const { return m_ViewProjectionMatrix;         }
		inline const glm::mat4 &GetPreviousViewProjectionMatrix() const { return m_PreviousViewProjectionMatrix; }
	private:
		void UpdateExposure();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();

		bool OnMouseMove(MouseMoveEvent     &event);
		bool OnMouseScroll(MouseScrollEvent &event);
	private:
		glm::vec3          m_Position;
		EulerAngles        m_Rotation;

		//Unit: millimeters
		float              m_FocalLength;
		//Unit: millimeters
		float              m_SensorHeight;
		float              m_AspectRatio;
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

		bool               m_ProjectionMatrixRequiresUpdate;
		glm::mat4          m_ViewMatrix;
		glm::mat4          m_ProjectionMatrix;
		glm::mat4          m_ViewProjectionMatrix;
		glm::mat4          m_PreviousViewProjectionMatrix;
	};
}