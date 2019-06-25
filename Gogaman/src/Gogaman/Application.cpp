#include "pch.h"
#include "Application.h"

#include "Base.h"
#include "Config.h"
#include "Events/EventManager.h"
#include "Logging/Log.h"

#include "SpatialComponent.h"
#include "Graphics/RenderableComponent.h"

#include "Graphics/ModelMatrixSystem.h"
#include "Graphics/RenderingSystem.h"

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
		GetWorld().AddSystem(std::make_unique<ModelMatrixSystem>());
		GetWorld().AddSystem(std::make_unique<RenderingSystem>());
		GetWorld().Initialize();

		GetWindow().EnableVerticalSync();
		
		FlexData::FlexData data = std::move(FlexData::ImportFlexData("D:/dev/monkey.flex"));
		FlexData::PrintFlexData(data);

		for(int i = 0; i < 1; i++)
		{
			for(auto &j : data.meshes)
			{
				Entity testEntity = GetWorld().CreateEntity();

				//Set spatial component data
				SpatialComponent testSpatialComponent;
				testSpatialComponent.position      = glm::vec3(0.0f, 0.0f, 0.0f);
				testSpatialComponent.scale         = glm::vec3(1.0f);
				testSpatialComponent.rotation      = glm::vec3(0.0f);
				testSpatialComponent.rotationAngle = 0.0f;
				GetWorld().AddComponent(testEntity.identifier, std::move(testSpatialComponent));

				//Set renderable component data
				RenderableComponent testRenderableComponent;
				testRenderableComponent.vertexArrayBuffer = std::make_unique<VertexArrayBuffer>();
				testRenderableComponent.vertexBuffer      = std::make_unique<VertexBuffer>();
				testRenderableComponent.indexBuffer       = std::make_unique<IndexBuffer>();
				
				testRenderableComponent.vertexBuffer->UploadData(FLEX_VERTEX_DATA_SIZE * j.vertexBufferData.size(), j.vertexBufferData.data());
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

				testRenderableComponent.indexBuffer->UploadData(j.indexBufferData.size(), j.indexBufferData.data());
				testRenderableComponent.vertexArrayBuffer->SetIndexBuffer(*testRenderableComponent.indexBuffer.get());

				testRenderableComponent.material.albedo = std::make_unique<Texture2D>();
				testRenderableComponent.material.albedo->internalFormat    = TextureInternalFormat::RGBW8;
				testRenderableComponent.material.albedo->format            = TextureFormat::XYZW;
				testRenderableComponent.material.albedo->interpolationMode = TextureInterpolationMode::Trilinear;
				testRenderableComponent.material.albedo->levels            = 0;
				testRenderableComponent.material.albedo->Generate(data.materials[0].albedo.width, data.materials[0].albedo.height, data.materials[0].albedo.data);

				testRenderableComponent.material.normal = std::make_unique<Texture2D>();
				testRenderableComponent.material.normal->internalFormat    = TextureInternalFormat::XYZW8;
				testRenderableComponent.material.normal->format            = TextureFormat::XYZW;
				testRenderableComponent.material.normal->interpolationMode = TextureInterpolationMode::Trilinear;
				testRenderableComponent.material.normal->levels            = 0;
				testRenderableComponent.material.normal->Generate(data.materials[0].normal.width, data.materials[0].normal.height, data.materials[0].normal.data);

				testRenderableComponent.material.roughness = std::make_unique<Texture2D>();
				testRenderableComponent.material.roughness->internalFormat    = TextureInternalFormat::X8;
				testRenderableComponent.material.roughness->format            = TextureFormat::X;
				testRenderableComponent.material.roughness->interpolationMode = TextureInterpolationMode::Trilinear;
				testRenderableComponent.material.roughness->levels            = 0;
				testRenderableComponent.material.roughness->Generate(data.materials[0].roughness.width, data.materials[0].roughness.height, data.materials[0].roughness.data);

				testRenderableComponent.material.metalness = std::make_unique<Texture2D>();
				testRenderableComponent.material.metalness->internalFormat    = TextureInternalFormat::X8;
				testRenderableComponent.material.metalness->format            = TextureFormat::X;
				testRenderableComponent.material.metalness->interpolationMode = TextureInterpolationMode::Trilinear;
				testRenderableComponent.material.metalness->levels            = 0;
				testRenderableComponent.material.metalness->Generate(data.materials[0].metalness.width, data.materials[0].metalness.height, data.materials[0].metalness.data);
				
				testRenderableComponent.material.emissivity = std::make_unique<Texture2D>();
				testRenderableComponent.material.emissivity->internalFormat    = TextureInternalFormat::X8;
				testRenderableComponent.material.emissivity->format            = TextureFormat::X;
				testRenderableComponent.material.emissivity->interpolationMode = TextureInterpolationMode::Trilinear;
				testRenderableComponent.material.emissivity->levels            = 0;
				testRenderableComponent.material.emissivity->Generate(data.materials[0].emissivity.width, data.materials[0].emissivity.height, data.materials[0].emissivity.data);

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