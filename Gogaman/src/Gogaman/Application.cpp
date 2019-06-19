#include "pch.h"
#include "Application.h"

#include "Base.h"
#include "Config.h"
#include "Events/EventManager.h"
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
		GetWorld().AddSystem(std::make_unique<RenderSystem>());
		GetWorld().Initialize();

		GetWindow().DisableVerticalSync();
		/*
		FlexData::FlexData data = std::move(FlexData::ImportFlexData("D:/dev/testScene.flex"));
		for(auto &i : data.meshes)
		{
			Entity gogaEntity = GetWorld().CreateEntity();

			SpatialComponent testSpatialComponent;
			testSpatialComponent.position      = glm::vec3(0.0f);
			testSpatialComponent.scale         = glm::vec3(1.0f);
			testSpatialComponent.rotation      = glm::vec3(0.0f);
			testSpatialComponent.rotationAngle = 0.0f;
			GetWorld().AddComponent(gogaEntity.identifier, std::move(testSpatialComponent));

			RenderableComponent testRenderableComponent;
			testRenderableComponent.data = i;
			testRenderableComponent.material.albedo = glm::vec3(0.0f, 0.9f, 0.1f);
			testRenderableComponent.material.roughness = 0.2f;
			GetWorld().AddComponent(gogaEntity.identifier, std::move(testRenderableComponent));
		}
		*/
		while(m_IsRunning)
		{
			GetWorld().Update();
			GetWorld().Render();
			
			GetWindow().Update();

			EventManager::GetInstance().DispatchEvents();
		}

		GetWorld().Shutdown();
		Window::Shutdown();
	}
}