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

		GetWindow().EnableVerticalSync();
		
		FlexData::FlexData data = std::move(FlexData::ImportFlexData("D:/dev/testScene.flex"));
		for(int i = 0; i < 1; i++)
		{
			for(auto &i : data.meshes)
			{
				Entity testEntity = GetWorld().CreateEntity();

				//Set spatial component data
				SpatialComponent testSpatialComponent;
				testSpatialComponent.position      = glm::vec3(tan(glfwGetTime() * 10410) * 0.9f);
				testSpatialComponent.scale         = glm::vec3(abs(sin(glfwGetTime() * 19600)));
				testSpatialComponent.rotation      = glm::vec3(cos(glfwGetTime() * 12500));
				testSpatialComponent.rotationAngle = abs(tan(glfwGetTime() * 108920)) * 3;
				GetWorld().AddComponent(testEntity.identifier, std::move(testSpatialComponent));

				//Set renderable component data
				RenderableComponent testRenderableComponent;

				testRenderableComponent.vertexArrayBuffer = std::make_unique<VertexArrayBuffer>();
				testRenderableComponent.vertexBuffer      = std::make_unique<VertexBuffer>();
				testRenderableComponent.indexBuffer       = std::make_unique<IndexBuffer>();
				
				testRenderableComponent.vertexBuffer->UploadData(FLEX_VERTEX_DATA_SIZE * i.vertexBufferData.size(), i.vertexBufferData.data());
				testRenderableComponent.vertexBuffer->SetLayout({
					//Position
					{ ShaderDataType::Float3 },
					//UV
					{ ShaderDataType::Float2 },
					//Normal
					{ ShaderDataType::Float3 },
					//Tangent
					{ ShaderDataType::Float3 }
				});

				testRenderableComponent.vertexArrayBuffer->AddVertexBuffer(*testRenderableComponent.vertexBuffer.get());

				testRenderableComponent.indexBuffer->UploadData(i.indexBufferData.size(), i.indexBufferData.data());
				testRenderableComponent.vertexArrayBuffer->SetIndexBuffer(*testRenderableComponent.indexBuffer.get());

				testRenderableComponent.material.albedo    = glm::vec3(abs(cos(glfwGetTime() * 14490)), abs(tan(glfwGetTime() * 490470)), abs(sin(glfwGetTime() * 87240)));
				testRenderableComponent.material.roughness = abs(tan(glfwGetTime() * 131015));
				testRenderableComponent.material.metalness = abs(sin(glfwGetTime() * 551178));

				GetWorld().AddComponent(testEntity.identifier, std::move(testRenderableComponent));
			}
		}
		
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