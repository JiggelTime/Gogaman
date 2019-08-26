#include "pch.h"
#include "Application.h"

#include "Core/Base.h"
#include "Core/Config.h"
#include "Core/Time.h"
#include "Logging/Log.h"

#include "Input/Input.h"

#include "Events/EventManager.h"

#include "SpatialComponent.h"
#include "Rendering/RenderableComponent.h"
#include "Rendering/LightComponent.h"

#include "Rendering/ModelMatrixSystem.h"
#include "Rendering/RenderingSystem.h"

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
		GM_CONFIG.vSync ? GetWindow().EnableVerticalSynchronization() : GetWindow().DisableVerticalSynchronization();

		m_World.AddSystem(std::make_unique<ModelMatrixSystem>());
		m_World.AddSystem(std::make_unique<RenderingSystem>());
		m_World.Initialize();
	}

	Application::~Application()
	{}
	
	void Application::Run()
	{
		GM_LOG_CORE_SET_LEVEL(Info);

		FlexData::FlexData data = FlexData::ImportFlexData("D:/dev/testScene/testScene.flex");
		//FlexData::PrintFlexData(data);

		for(const auto &j : data.meshes)
		{
			Entity testMeshEntity = m_World.CreateEntity();

			//Set spatial component data
			SpatialComponent testSpatialComponent;
			testSpatialComponent.position      = glm::vec3(j.transform.position[0], j.transform.position[1], j.transform.position[2]);
			//testSpatialComponent.position = glm::vec3(0.0f + (float)i, 0.0f, 0.0f);
			testSpatialComponent.rotation      = glm::vec3(j.transform.rotation[0], j.transform.rotation[1], j.transform.rotation[2]);
			testSpatialComponent.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
			testSpatialComponent.rotationAngle = 0.0f;
			testSpatialComponent.scale         = glm::vec3(j.transform.scale[0], j.transform.scale[1], j.transform.scale[2]);
			//testSpatialComponent.scale = glm::vec3(1.0f);
			m_World.AddComponent<SpatialComponent>(testMeshEntity.identifier, std::move(testSpatialComponent));

			//Set renderable component data
			RenderableComponent testRenderableComponent;
			testRenderableComponent.vertexArrayBuffer = std::make_unique<VertexArrayBuffer>();
			testRenderableComponent.vertexBuffer      = std::make_unique<VertexBuffer>();
			testRenderableComponent.indexBuffer       = std::make_unique<IndexBuffer>();
			
			testRenderableComponent.vertexBuffer->UploadData(FLEX_VERTEX_DATA_SIZE * j.vertexBuffer.size(), j.vertexBuffer.data());
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

			testRenderableComponent.indexBuffer->UploadData(j.indexBuffer.size(), j.indexBuffer.data());
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
			//Bounding sphere
			testRenderableComponent.boundingSphere.position = glm::vec3(j.boundingSphere.position[0], j.boundingSphere.position[1], j.boundingSphere.position[2]);
			testRenderableComponent.boundingSphere.radius   = j.boundingSphere.radius;
			//AABB
			testRenderableComponent.axisAlignedBoundingBox.minimum = glm::vec3(j.axisAlignedBoundingBox.minimum[0], j.axisAlignedBoundingBox.minimum[1], j.axisAlignedBoundingBox.minimum[2]);
			testRenderableComponent.axisAlignedBoundingBox.maximum = glm::vec3(j.axisAlignedBoundingBox.maximum[0], j.axisAlignedBoundingBox.maximum[1], j.axisAlignedBoundingBox.maximum[2]);

			m_World.AddComponent(testMeshEntity.identifier, std::move(testRenderableComponent));
		}

		for(const auto &i : data.pointLights)
		{
			Entity testPointLightEntity = m_World.CreateEntity();

			PointLightComponent testPointLightComponent;
			testPointLightComponent.position = glm::vec3(i.position[0], i.position[1], i.position[2]);
			testPointLightComponent.radiance = glm::vec3(i.radiance[0], i.radiance[1], i.radiance[2]);

			m_World.AddComponent(testPointLightEntity.identifier, std::move(testPointLightComponent));
		}
		
		while(m_IsRunning)
		{
			Time::Update();

			m_World.Update();
			m_World.Render();
			
			GetWindow().Update();

			EventManager::GetInstance().DispatchEvents();
			Input::Update();
		}

		m_World.Shutdown();
		Window::Shutdown();
	}
}