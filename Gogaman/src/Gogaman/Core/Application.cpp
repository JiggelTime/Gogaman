#include "pch.h"
#include "Application.h"

#include "Logging/Log.h"
#include "Base.h"

#include "Config.h"
#include "Time.h"

#include "Gogaman/Input/Input.h"

#include "Gogaman/Events/EventManager.h"

#include "Gogaman/BoundingVolumeSystem.h"
#include "Gogaman/Rendering/ModelMatrixSystem.h"
#include "Gogaman/Rendering/RenderingSystem.h"

#include "Gogaman/Utilities/Algorithm/DirectedGraph.h"

namespace Gogaman
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
		: m_IsRunning(true)
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct application | Instance already exists");
		s_Instance = this;

		GM_LOG_CORE_SET_LEVEL(Info);

		m_Window = std::make_unique<Window>("Gogaman", GM_CONFIG.screenWidth, GM_CONFIG.screenHeight, VerticalSynchronization::Disabled);

		m_World.AddSystem(std::make_unique<ModelMatrixSystem>());
		m_World.AddSystem(std::make_unique<BoundingVolumeSystem>());
		m_World.AddSystem(std::make_unique<RenderingSystem>());
		m_World.Initialize();
	}

	Application::~Application()
	{
		m_World.Shutdown();

		m_Window.reset();
		Window::Shutdown();
	}
	
	void Application::Run()
	{
		while(m_IsRunning)
		{
			Time::Update();
			Input::Update();

			m_World.Update();
			m_World.Render();
			
			m_Window->Update();

			EventManager::GetInstance().DispatchEvents();
		}
	}
}