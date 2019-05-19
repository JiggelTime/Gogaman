#include "pch.h"
#include "Application.h"
#include "Logging/Log.h"

#include "Platform/OpenGL/Renderer.h"
#include "Events/KeyboardEvent.h"

namespace Gogaman
{
	Application::Application()
	{}

	Application::~Application()
	{}

	void Application::Run()
	{
		Renderer gogaRenderer("Gogaman");
		KeyPressEvent goga(3, 1);
		EventQueue::Enqueue(goga);

		while(true)
		{
			gogaRenderer.Render();	

			EventQueue::DispatchEvents();
		}

		EventQueue::Clear();
	}
}