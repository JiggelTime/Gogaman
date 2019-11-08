#pragma once

#include <glm.hpp>

#define GM_VSYNC_MODE DoubleBuffered

namespace Gogaman
{
	struct Config
	{
		static Config &GetConfig()
		{
			static Config instance;
			return instance;
		}

		//Logging
			const bool        logToFile   = true;
			const std::string logFilepath = "Gogaman.log";

		//Window
			//Rendering resolution scale
			const float resScale        = 1.0f;
			//Window resolution
			const uint16_t screenWidth  = 2560;
			const uint16_t screenHeight = 1440;
			const float aspectRatio     = float(screenWidth) / float(screenHeight);

		//Controls
			const float mouseSensitivity    = 0.1f;
			const float cameraMovementSpeed = 3.0f;

		//Depth of field
			bool dof = false;
			bool dofKeyPressed = false;
			const float dofResScale = 1.0f;
			//Distance to the point of focus (meters)
			const float focalDistance = 10.0f;
			//Ratio of the aperture (f-stop)
			const float fStop = 0.01f;
			//Distance between the lens and the film (mm)
			const float focalLength = 50.0f;
	};
}

#define GM_CONFIG Gogaman::Config::GetConfig()