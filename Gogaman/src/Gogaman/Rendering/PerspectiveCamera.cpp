#include "pch.h"
#include "PerspectiveCamera.h"

#include "Gogaman/Core/Config.h"
#include "Gogaman/Core/Time.h"

#include "Gogaman/Input/Input.h"

#include "Gogaman/Events/EventDispatcher.h"

namespace Gogaman
{
	PerspectiveCamera::PerspectiveCamera(const glm::vec3 &position, const EulerAngles &rotation, const float focalLength, const float sensorHeight, const float aspectRatio, const float aperture, const float sensitivity, const float shutterSpeed, const float nearPlane, const float farPlane)
		: m_Position(position), m_Rotation(rotation), m_FocalLength(focalLength), m_SensorHeight(sensorHeight), m_AspectRatio(aspectRatio), m_Aperture(aperture), m_Sensitivity(sensitivity), m_ShutterSpeed(shutterSpeed), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		m_ExposureRequiresUpdate         = true;
		m_ProjectionMatrixRequiresUpdate = true;
		Update();
	}

	PerspectiveCamera::~PerspectiveCamera()
	{}

	void PerspectiveCamera::Update()
	{
		if(m_ExposureRequiresUpdate)
			UpdateExposure();

		UpdateViewMatrix();

		if(m_ProjectionMatrixRequiresUpdate)
			UpdateProjectionMatrix();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		m_Frustum = RectangularFrustum(m_ViewProjectionMatrix);
	}

	void PerspectiveCamera::UpdateExposure()
	{
		float exposureValue100      = log2(pow(m_Aperture, 2) / m_ShutterSpeed * 100.0f / m_Sensitivity);
		m_Exposure                  = 1.0f / (exp2(exposureValue100) * 1.2f);
		m_BloomLuminanceCoeffecient = exp2(exposureValue100 - 3.0f);
		m_ExposureRequiresUpdate    = false;
	}

	void PerspectiveCamera::UpdateViewMatrix()
	{
		//UVN system
		float cosPitch = cosf(glm::radians(m_Rotation.pitch));
		glm::vec3 n;
		n.x = cosf(glm::radians(m_Rotation.yaw)) * cosPitch;
		n.y = sinf(glm::radians(m_Rotation.pitch));
		n.z = sinf(glm::radians(m_Rotation.yaw)) * cosPitch;
		n = glm::normalize(n);

		glm::vec3 u = glm::normalize(glm::cross(n, glm::vec3(0.0f, 1.0f, 0.0f)));

		float velocity = GM_CONFIG.cameraMovementSpeed * Time::GetDeltaTime();
		//Forward
		if(Input::IsKeyPressed(GM_KEY_W))
			m_Position += n * velocity;
		//Backward
		if(Input::IsKeyPressed(GM_KEY_S))
			m_Position -= n * velocity;
		//Left
		if(Input::IsKeyPressed(GM_KEY_A))
			m_Position -= u * velocity;
		//Right
		if(Input::IsKeyPressed(GM_KEY_D))
			m_Position += u * velocity;

		glm::vec3 v = glm::normalize(glm::cross(u, n));
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + n, v);
	}

	void PerspectiveCamera::UpdateProjectionMatrix()
	{
		//Unit: radians
		float verticalAngleOfView      = atanf(m_SensorHeight / (m_FocalLength * 2.0f)) * 2.0f;
		m_ProjectionMatrix             = glm::perspective(verticalAngleOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
		m_PreviousViewProjectionMatrix = m_ViewProjectionMatrix;

		m_ProjectionMatrixRequiresUpdate = false;
	}

	void PerspectiveCamera::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseMoveEvent>(GM_BIND_EVENT_CALLBACK(PerspectiveCamera::OnMouseMove));
	}

	bool PerspectiveCamera::OnMouseMove(MouseMoveEvent &event)
	{
		glm::vec2 movementOffset = Input::GetDeltaMousePosition() * GM_CONFIG.mouseSensitivity;

		m_Rotation.yaw = fmod(m_Rotation.yaw + movementOffset.x, 360.0f);

		m_Rotation.pitch += movementOffset.y;
		if(m_Rotation.pitch < -89.0f)
			m_Rotation.pitch = -89.0f;
		else if(m_Rotation.pitch > 89.0f)
			m_Rotation.pitch = 89.0f;

		return true;
	}

	void PerspectiveCamera::SetFocalLength(const float focalLength)
	{
		m_FocalLength                    = focalLength;
		m_ProjectionMatrixRequiresUpdate = true;
	}

	void PerspectiveCamera::SetSensorHeight(const float sensorHeight)
	{
		m_SensorHeight                   = sensorHeight;
		m_ExposureRequiresUpdate         = true;
		m_ProjectionMatrixRequiresUpdate = true;
	}

	void PerspectiveCamera::SetAspectRatio(const float aspectRatio)
	{
		m_AspectRatio                    = aspectRatio;
		m_ProjectionMatrixRequiresUpdate = true;
	}

	void PerspectiveCamera::SetAperture(const float aperture)
	{
		m_Aperture               = aperture;
		m_ExposureRequiresUpdate = true;
	}

	void PerspectiveCamera::SetSensitivity(const float sensitivity)
	{
		m_Sensitivity            = sensitivity;
		m_ExposureRequiresUpdate = true;
	}

	void PerspectiveCamera::SetShutterSpeed(const float shutterSpeed)
	{
		m_ShutterSpeed           = shutterSpeed;
		m_ExposureRequiresUpdate = true;
	}
}