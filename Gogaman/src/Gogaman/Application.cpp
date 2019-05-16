#include "pch.h"
#include "Application.h"
#include "Logging/Log.h"

#include "Platform/OpenGL/Renderer.h"
#include "Events/Event.h"
#include "Events/KeyboardEvent.h"

namespace Gogaman
{
	Application::Application()
	{}

	Application::~Application()
	{}

	void Application::Run()
	{
		//Renderer gogaRenderer("Gogaman");

		while(true)
		{
			//gogaRenderer.Render();
		}
	}
}