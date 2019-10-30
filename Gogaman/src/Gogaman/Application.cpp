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

#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderGraph/RenderGraphStageBuilder.h"

namespace Gogaman
{
	Application *Application::s_Instance = nullptr;

	Application::Application()
		: m_IsRunning(true)
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct application: instance already exists");
		s_Instance = this;

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
		//GM_LOG_CORE_SET_LEVEL(Error);

		RenderGraph::Graph graph;

		RenderGraph::Stage &gBuffer = graph.CreateStage();
		RenderGraph::StageBuilder builder(&graph, &gBuffer);
		builder.CreateTextureResource("albedo");
		builder.CreateTextureResource("normals");
		builder.CreateTextureResource("roughness");

		RenderGraph::Stage &deferredShading = graph.CreateStage();
		RenderGraph::StageBuilder builder2(&graph, &deferredShading);
		builder2.ReadTextureResource("albedo");
		builder2.ReadTextureResource("normals");
		builder2.ReadTextureResource("roughness");
		builder2.CreateTextureResource("shadedImage");

		RenderGraph::Stage &taa = graph.CreateStage();
		RenderGraph::StageBuilder builder3(&graph, &taa);
		builder3.WriteTextureResource("shadedImage", "antiAliasedImage");

		graph.SetBackBuffer("antiAliasedImage");

		graph.Compile();
		graph.Log();

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