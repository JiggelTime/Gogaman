#include "pch.h"
#include "Application.h"
#include "Logging/Logger.h"
#include "graphics/Lights/PointLight.h"

namespace Gogaman
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		GM_LOG_SET_LEVEL(LogLevel::LogError);
		GM_LOG_ERROR("Log level: %d", (int)GM_LOG_GET_LEVEL);
		GM_LOG_SET_LEVEL(LogLevel::LogInfo);
		GM_LOG_WARNING("Log level: %d", (int)GM_LOG_GET_LEVEL);

		while (true);
	}
}