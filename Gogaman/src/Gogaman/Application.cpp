#include "pch.h"
#include "Application.h"

#include "Base.h"
#include "Config.h"
#include "Events/EventQueue.h"
#include "Logging/Log.h"

#include "ECS/RenderableComponent.h"
#include "ECS/RenderSystem.h"

#include <FlexData.h>

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
		std::unique_ptr<System> testSystem = std::make_unique<RenderSystem>();
		GetWorld().AddSystem(std::move(testSystem));

		GetWorld().Initialize();

		GetWindow().DisableVerticalSync();

		FlexData::FlexData data = std::move(FlexData::ImportFlexData("D:/dev/test.flex"));
		for(auto &i : data.meshes)
		{
			Entity gogaEntity = GetWorld().CreateEntity();
			RenderableComponent testComp;
			testComp.data = i;
			GetWorld().AddComponent(gogaEntity.identifier, std::move(testComp));
		}

		while(m_IsRunning)
		{
			GetWindow().Update();
			GetWorld().Update();

			GetWorld().Render();

			EventQueue::GetInstance().DispatchEvents();
		}

		GetWorld().Shutdown();
	}
}