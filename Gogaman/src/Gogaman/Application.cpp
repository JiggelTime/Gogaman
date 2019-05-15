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

		EventDispatcher goga(KeyPressEvent);
		goga.Dispatch(GM_BIND_EVENT_CALLBACK(OnEvent));

		while(true)
		{
			//gogaRenderer.Render();
		}
	}
}