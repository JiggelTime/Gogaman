#include "pch.h"
#include "Application.h"

#include "Core/Base.h"
#include "Core/Logging/Log.h"

#include "Core/Config.h"
#include "Core/Time.h"

#include "Input/Input.h"

#include "Events/EventManager.h"

#include "Rendering/ModelMatrixSystem.h"
#include "BoundingVolumeSystem.h"
#include "Rendering/RenderingSystem.h"

namespace Gogaman
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
		: m_IsRunning(true)
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct application: instance already exists");
		s_Instance = this;

		GM_LOG_CORE_SET_LEVEL(Info);

		m_Window = std::unique_ptr<Window>(Window::Create("Gogaman", GM_CONFIG.screenWidth, GM_CONFIG.screenHeight));
		GM_CONFIG.vSync ? GetWindow().EnableVerticalSynchronization() : GetWindow().DisableVerticalSynchronization();

		m_World.AddSystem(std::make_unique<ModelMatrixSystem>());
		m_World.AddSystem(std::make_unique<BoundingVolumeSystem>());
		m_World.AddSystem(std::make_unique<RenderingSystem>());
		m_World.Initialize();
	}

	Application::~Application()
	{}
	
	void Application::Run()
	{
		while(m_IsRunning)
		{
			Time::Update();
			Input::Update();

			m_World.Update();
			m_World.Render();
			
			GetWindow().Update();

			EventManager::GetInstance().DispatchEvents();
		}

		m_World.Shutdown();
		Window::Shutdown();
	}
}