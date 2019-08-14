#include "pch.h"
#include "PerspectiveCamera.h"

namespace Gogaman
{
	PerspectiveCamera::PerspectiveCamera(const glm::vec3 &position, const glm::vec4 &rotation, const float focalLength, const float aperture, const float sensitivity, const float shutterSpeed, const float nearPlane, const float farPlane)
		: m_Position(position), m_Rotation(rotation), m_FocalLength(focalLength), m_Aperture(aperture), m_Sensitivity(sensitivity), m_ShutterSpeed(shutterSpeed), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		UpdateExposure();
		UpdateMatrices();
	}

	PerspectiveCamera::~PerspectiveCamera()
	{}

	void PerspectiveCamera::Update()
	{
		if(m_ExposureRequiresUpdate)
			UpdateExposure();

		if(m_MatricesRequireUpdate)
			UpdateMatrices();
	}

	void PerspectiveCamera::UpdateExposure()
	{
		float exposureValue100      = log2(pow(m_Aperture, 2) / m_ShutterSpeed * 100.0f / m_Sensitivity);
		m_Exposure                  = 1.0f / (exp2(exposureValue100) * 1.2f);
		m_BloomLuminanceCoeffecient = exp2(exposureValue100 - 3.0f);
		m_ExposureRequiresUpdate    = false;
	}

	void PerspectiveCamera::UpdateMatrices()
	{
		//m_ViewMatrix       = glm::lookAt();
		//m_ProjectionMatrix = glm::perspective();

		m_PreviousViewProjectionMatrix = m_ViewProjectionMatrix;
		m_ViewProjectionMatrix         = m_ProjectionMatrix * m_ViewMatrix;
		m_MatricesRequireUpdate        = false;
	}

	void PerspectiveCamera::SetPosition(const glm::vec3 &position)
	{
		m_Position              = position;
		m_MatricesRequireUpdate = true;
	}

	void PerspectiveCamera::SetRotation(const glm::vec4 &rotation)
	{
		m_Rotation              = rotation;
		m_MatricesRequireUpdate = true;
	}

	void PerspectiveCamera::SetFocalLength(const float focalLength)
	{
		m_FocalLength            = focalLength;
		m_ExposureRequiresUpdate = true;
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