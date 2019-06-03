#include "pch.h"
#include "Application.h"

#include "Config.h"
#include "Window.h"
#include "Logging/Log.h"

#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"

#include "Platform/OpenGL/Renderer.h"

namespace Gogaman
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
		: m_IsRunning(true)
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct application: instance already exists");

		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create("Gogaman", GM_CONFIG.screenWidth, GM_CONFIG.screenHeight));
	}

	Application::~Application()
	{}

	void Application::Run()
	{
		GetWindow().DisableVerticalSync();

		Renderer gogaRenderer(GetWindow());

		while(m_IsRunning)
		{
			gogaRenderer.Render();

			GetWindow().Update();

			EventQueue::GetInstance().DispatchEvents();
		}
	}
}