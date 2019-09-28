#include "pch.h"
#include "RenderingSystem.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"
#include "Gogaman/Core/Time.h"

#include "Gogaman/Application.h"

#include "Gogaman/Input/Input.h"

#include "Gogaman/Events/EventDispatcher.h"

#include "Gogaman/ECS/Entity.h"
#include "Gogaman/ECS/World.h"

#include "Gogaman/SpatialComponent.h"
#include "Gogaman/BoundingVolumeComponent.h"
#include "RenderableComponent.h"
#include "LightComponent.h"

#include <FlexData.h>

#include <glm.hpp>

#define GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX        0
#define GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX       1
#define GM_RENDERING_SYSTEM_DIRECTIONAL_LIGHT_GROUP_INDEX 2

#if GM_RENDERING_API == GM_RENDERING_API_OPENGL
	#define GM_SHADERS_DIRECTORY std::string("D:/dev/Gogaman/Gogaman/src/Platform/OpenGL/Shaders/")
#else
	#error
#endif

#define GM_BRDF_LUT_XY 512

namespace Gogaman
{
	RenderingSystem::RenderingSystem()
	{
		EntityGroup renderableGroup;
		renderableGroup.AddComponentType<BoundingVolumeComponent>();
		renderableGroup.AddComponentType<RenderableComponent>();
		AddEntityGroup(std::move(renderableGroup));

		EntityGroup pointLightGroup;
		pointLightGroup.AddComponentType<PointLightComponent>();
		AddEntityGroup(std::move(pointLightGroup));

		EntityGroup directionalLightGroup;
		directionalLightGroup.AddComponentType<DirectionalLightComponent>();
		AddEntityGroup(std::move(directionalLightGroup));
	}

	void RenderingSystem::Initialize()
	{
		m_RenderResolutionWidth  = (int)(Application::GetInstance().GetWindow().GetWidth()  * GM_CONFIG.resScale);
		m_RenderResolutionHeight = (int)(Application::GetInstance().GetWindow().GetHeight() * GM_CONFIG.resScale);

		m_Camera.SetFocalLength(20.0f);
		m_Camera.SetSensorHeight(24.0f);
		m_Camera.SetAspectRatio(GM_CONFIG.aspectRatio);

		InitializeRenderSurfaces();
		InitializeShaders();

		ImportFlexData();

		m_FullscreenTriangle = std::make_unique<FullscreenTriangle>();

		//Generate BRDF LUT for split-sum approximation
		glViewport(0, 0, GM_BRDF_LUT_XY, GM_BRDF_LUT_XY);
		RenderSurface &precomputedBRDF = GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_PrecomputedBRDF);
		precomputedBRDF.Bind();
		m_ShaderManager.Get(m_PrecomputedBRDF_Shader).Bind();
		RenderFullscreenWindow();
	}

	void RenderingSystem::InitializeRenderSurfaces()
	{
		//Precomputed BRDF
		RenderSurface &precomputedBRDF = GM_RENDERING_CONTEXT.GetRenderSurfaces().Create(m_PrecomputedBRDF);

		m_BRDF_LUT.internalFormat    = TextureInternalFormat::XY16F;
		m_BRDF_LUT.format            = TextureFormat::XY;
		m_BRDF_LUT.interpolationMode = TextureInterpolationMode::Bilinear;
		m_BRDF_LUT.Generate(GM_BRDF_LUT_XY, GM_BRDF_LUT_XY);
		precomputedBRDF.AddColorBuffer(m_BRDF_LUT);
		
		//G-Buffer
		RenderSurface &geometryBuffer = GM_RENDERING_CONTEXT.GetRenderSurfaces().Create(m_G_Buffer);

		Texture2D &geometryBufferPositionMetalness = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_G_BufferPositionMetalness);
		geometryBufferPositionMetalness.internalFormat    = TextureInternalFormat::XYZW16F;
		geometryBufferPositionMetalness.format            = TextureFormat::XYZW;
		geometryBufferPositionMetalness.interpolationMode = TextureInterpolationMode::Point;
		geometryBufferPositionMetalness.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		geometryBuffer.AddColorBuffer(geometryBufferPositionMetalness);

		Texture2D &geometryBufferNormal = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_G_BufferNormal);
		geometryBufferNormal.internalFormat    = TextureInternalFormat::XY16F;
		geometryBufferNormal.format            = TextureFormat::XY;
		geometryBufferNormal.interpolationMode = TextureInterpolationMode::Point;
		geometryBufferNormal.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		geometryBuffer.AddColorBuffer(geometryBufferNormal);

		Texture2D &geometryBufferAlbedoEmissivityRoughness = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_G_BufferAlbedoEmissivityRoughness);
		geometryBufferAlbedoEmissivityRoughness.internalFormat    = TextureInternalFormat::XYZW8;
		geometryBufferAlbedoEmissivityRoughness.format            = TextureFormat::XYZW;
		geometryBufferAlbedoEmissivityRoughness.interpolationMode = TextureInterpolationMode::Bilinear;
		geometryBufferAlbedoEmissivityRoughness.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		geometryBuffer.AddColorBuffer(geometryBufferAlbedoEmissivityRoughness);

		Texture2D &geometryBufferVelocity = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_G_BufferVelocity);
		geometryBufferVelocity.internalFormat    = TextureInternalFormat::XY16F;
		geometryBufferVelocity.format            = TextureFormat::XY;
		geometryBufferVelocity.interpolationMode = TextureInterpolationMode::Point;
		geometryBufferVelocity.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		geometryBuffer.AddColorBuffer(geometryBufferVelocity);

		Texture2D &geometryBufferDepth = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_G_BufferDepth);
		geometryBufferDepth.internalFormat    = TextureInternalFormat::Depth32;
		geometryBufferDepth.format            = TextureFormat::X;
		geometryBufferDepth.interpolationMode = TextureInterpolationMode::Point;
		geometryBufferDepth.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		geometryBuffer.AddDepthBuffer(geometryBufferDepth);

		//Final image
		RenderSurface &finalBuffer = GM_RENDERING_CONTEXT.GetRenderSurfaces().Create(m_FinalBuffer);

		Texture2D &finalImage = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(m_FinalImage);
		finalImage.internalFormat    = TextureInternalFormat::XYZW16F;
		finalImage.format            = TextureFormat::XYZW;
		finalImage.interpolationMode = TextureInterpolationMode::Bilinear;
		finalImage.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		finalBuffer.AddColorBuffer(finalImage);

		m_FinalImageDepth.internalFormat = TextureInternalFormat::Depth32;
		m_FinalImageDepth.Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		finalBuffer.AddDepthBuffer(m_FinalImageDepth);
	}

	void RenderingSystem::InitializeShaders()
	{
		m_PrecomputedBRDF_Shader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("precomputeBRDF.vs"), GM_SHADERS_DIRECTORY.append("precomputeBRDF.fs"));

		m_G_BufferShader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("gbuffershader.vs"), GM_SHADERS_DIRECTORY.append("gbuffershader.fs"));
		m_ShaderManager.Get(m_G_BufferShader).UploadUniform("materialAlbedo",     0);
		m_ShaderManager.Get(m_G_BufferShader).UploadUniform("materialNormal",     1);
		m_ShaderManager.Get(m_G_BufferShader).UploadUniform("materialRoughness",  2);
		m_ShaderManager.Get(m_G_BufferShader).UploadUniform("materialMetalness",  3);
		m_ShaderManager.Get(m_G_BufferShader).UploadUniform("materialEmissivity", 4);

		m_DeferredLightingShader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("directPBR.vs"), GM_SHADERS_DIRECTORY.append("directPBR.fs"));
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("gPositionMetalness",         0);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("gNormal",                    1);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("gAlbedoEmissivityRoughness", 2);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("BRDF_LUT",                   3);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("coneTracedDiffuse",          4);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("coneTracedSpecular",         5);
		m_ShaderManager.Get(m_DeferredLightingShader).UploadUniform("voxelTexture",               6);

		m_LightShader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("lampshader.vs"), GM_SHADERS_DIRECTORY.append("lampshader.fs"));
		
		m_SkyboxShader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("skyboxshader.vs"), GM_SHADERS_DIRECTORY.append("skyboxshader.fs"));
		m_ShaderManager.Get(m_SkyboxShader).UploadUniform("skybox", 0);

		m_PostprocessShader = m_ShaderManager.Create(GM_SHADERS_DIRECTORY.append("postprocess.vs"), GM_SHADERS_DIRECTORY.append("postprocess.fs"));
		m_ShaderManager.Get(m_PostprocessShader).UploadUniform("hdrTexture", 0);
	}

	void RenderingSystem::ImportFlexData()
	{
		World &world = Application::GetInstance().GetWorld();
		
		FlexData::FlexData data = FlexData::ImportFlexData("D:/dev/testScene/testScene.flex");
		//FlexData::PrintFlexData(data);

		m_Materials.reserve(data.materials.size());
		for(const auto &i : data.materials)
		{
			PBR_Material material;

			Texture2D &albedoTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(material.albedo);
			albedoTexture.internalFormat    = TextureInternalFormat::RGBW8;
			albedoTexture.format            = TextureFormat::XYZW;
			albedoTexture.interpolationMode = TextureInterpolationMode::Trilinear;
			albedoTexture.levels            = 0;
			albedoTexture.Generate(i.albedo.width, i.albedo.height, i.albedo.data);

			Texture2D &normalTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(material.normal);
			normalTexture.internalFormat    = TextureInternalFormat::XYZW8;
			normalTexture.format            = TextureFormat::XYZW;
			normalTexture.interpolationMode = TextureInterpolationMode::Trilinear;
			normalTexture.levels            = 0;
			normalTexture.Generate(i.normal.width, i.normal.height, i.normal.data);

			Texture2D &roughnessTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(material.roughness);
			roughnessTexture.internalFormat    = TextureInternalFormat::X8;
			roughnessTexture.format            = TextureFormat::X;
			roughnessTexture.interpolationMode = TextureInterpolationMode::Trilinear;
			roughnessTexture.levels            = 0;
			roughnessTexture.Generate(i.roughness.width, i.roughness.height, i.roughness.data);

			Texture2D &metalnessTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(material.metalness);
			metalnessTexture.internalFormat    = TextureInternalFormat::X8;
			metalnessTexture.format            = TextureFormat::X;
			metalnessTexture.interpolationMode = TextureInterpolationMode::Trilinear;
			metalnessTexture.levels            = 0;
			metalnessTexture.Generate(i.metalness.width, i.metalness.height, i.metalness.data);
			
			Texture2D &emissivityTexture = GM_RENDERING_CONTEXT.GetTexture2Ds().Create(material.emissivity);
			emissivityTexture.internalFormat    = TextureInternalFormat::X8;
			emissivityTexture.format            = TextureFormat::X;
			emissivityTexture.interpolationMode = TextureInterpolationMode::Trilinear;
			emissivityTexture.levels            = 0;
			emissivityTexture.Generate(i.emissivity.width, i.emissivity.height, i.emissivity.data);

			m_Materials.emplace_back(std::move(material));
		}

		for(const auto &i : data.meshes)
		{
			Entity mesh = world.CreateEntity();

			//Spatial component data
			SpatialComponent spatialComponent;
			spatialComponent.position      = glm::vec3(i.transform.position[0], i.transform.position[1], i.transform.position[2]);
			spatialComponent.rotation      = glm::vec3(i.transform.rotation[0], i.transform.rotation[1], i.transform.rotation[2]);
			spatialComponent.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
			spatialComponent.rotationAngle = 0.0f;
			spatialComponent.scale         = glm::vec3(i.transform.scale[0], i.transform.scale[1], i.transform.scale[2]);
			world.AddComponent<SpatialComponent>(mesh, std::move(spatialComponent));

			//Bounding volume component data
			BoundingVolumeComponent boundingVolumeComponent;
			//Bounding sphere
			boundingVolumeComponent.boundingSphere.position = glm::vec3(i.boundingSphere.position[0], i.boundingSphere.position[1], i.boundingSphere.position[2]);
			boundingVolumeComponent.boundingSphere.radius   = i.boundingSphere.radius;
			//AABB
			boundingVolumeComponent.axisAlignedBoundingBox.minimum = glm::vec3(i.axisAlignedBoundingBox.minimum[0], i.axisAlignedBoundingBox.minimum[1], i.axisAlignedBoundingBox.minimum[2]);
			boundingVolumeComponent.axisAlignedBoundingBox.maximum = glm::vec3(i.axisAlignedBoundingBox.maximum[0], i.axisAlignedBoundingBox.maximum[1], i.axisAlignedBoundingBox.maximum[2]);
			world.AddComponent<BoundingVolumeComponent>(mesh, std::move(boundingVolumeComponent));

			//Renderable component data
			RenderableComponent renderableComponent;

			VertexBuffer      &vertexBuffer      = GM_RENDERING_CONTEXT.GetVertexBuffers().Create(renderableComponent.vertexBuffer);
			IndexBuffer       &indexBuffer       = GM_RENDERING_CONTEXT.GetIndexBuffers().Create(renderableComponent.indexBuffer);
			VertexArrayBuffer &vertexArrayBuffer = GM_RENDERING_CONTEXT.GetVertexArrayBuffers().Create(renderableComponent.vertexArrayBuffer);
			
			vertexBuffer.UploadData(FLEX_VERTEX_DATA_SIZE * (uint32_t)i.vertexBuffer.size(), i.vertexBuffer.data());
			vertexBuffer.SetLayout({
				//Position
				{ ShaderDataType::Float3 },
				//UV
				{ ShaderDataType::Float2 },
				//Normal
				{ ShaderDataType::Float3 },
				//Tangent
				{ ShaderDataType::Float3 }
			});

			vertexArrayBuffer.AddVertexBuffer(vertexBuffer);

			indexBuffer.UploadData((uint32_t)i.indexBuffer.size(), i.indexBuffer.data());
			vertexArrayBuffer.SetIndexBuffer(indexBuffer);

			renderableComponent.materialIndex = i.materialIndex;
			world.AddComponent(mesh, std::move(renderableComponent));
		}

		for(const auto &i : data.pointLights)
		{
			Entity pointLight = world.CreateEntity();

			PointLightComponent pointLightComponent;
			pointLightComponent.position = glm::vec3(i.position[0], i.position[1], i.position[2]);
			pointLightComponent.radiance = glm::vec3(i.radiance[0], i.radiance[1], i.radiance[2]);

			world.AddComponent(pointLight, std::move(pointLightComponent));
		}
	}

	void RenderingSystem::RenderFullscreenWindow() const
	{
		m_FullscreenTriangle->GetVertexArrayBuffer().Bind();
		GM_RENDERING_CONTEXT.RenderIndexed(m_FullscreenTriangle->GetIndexBuffer().GetIndexCount());
	}

	void RenderingSystem::Update()
	{
		//Render resolution
		m_RenderResolutionWidth  = (int)(Application::GetInstance().GetWindow().GetWidth()  * GM_CONFIG.resScale);
		m_RenderResolutionHeight = (int)(Application::GetInstance().GetWindow().GetHeight() * GM_CONFIG.resScale);

		GLFWwindow *window = static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow());

		//Close window
		if(Input::IsKeyPressed(GM_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		//Reload shaders
		if(Input::IsKeyPressed(GM_KEY_R))
		{
			m_ShaderManager.ReloadAll();
			InitializeShaders();
		}
		
		//Toggle depth of field
		if(Input::IsKeyPressed(GM_KEY_U) && !GM_CONFIG.dof)
		{
			GM_CONFIG.dof = !GM_CONFIG.dof;
			GM_CONFIG.dofKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_U) == GLFW_RELEASE)
		{
			GM_CONFIG.dofKeyPressed = false;
		}

		//Enable/disable bloom
		if(Input::IsKeyPressed(GM_KEY_B) && !GM_CONFIG.bloomKeyPressed)
		{
			GM_CONFIG.bloom = !GM_CONFIG.bloom;
			GM_CONFIG.bloomKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_B) == GLFW_RELEASE)
		{
			GM_CONFIG.bloomKeyPressed = false;
		}

		//Enable/disable debug mode
		if(Input::IsKeyPressed(GM_KEY_G) && !GM_CONFIG.debugKeyPressed)
		{
			GM_CONFIG.debug = !GM_CONFIG.debug;
			GM_CONFIG.debugKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_G) == GLFW_RELEASE)
		{
			GM_CONFIG.debugKeyPressed = false;
		}

		//Enable/disable debug mode 2
		if(glfwGetKey(window, GM_KEY_H) == GLFW_PRESS && !GM_CONFIG.debug2KeyPressed)
		{
			GM_CONFIG.debug2 = !GM_CONFIG.debug2;
			GM_CONFIG.debug2KeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_H) == GLFW_RELEASE)
		{
			GM_CONFIG.debug2KeyPressed = false;
		}

		//Toggle spatio-temporal indirect lighting upscaling
		if(glfwGetKey(window, GM_KEY_Y) == GLFW_PRESS && !GM_CONFIG.giUpscalingKeyPressed)
		{
			GM_CONFIG.giUpscaling = !GM_CONFIG.giUpscaling;
			GM_CONFIG.giUpscalingKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_Y) == GLFW_RELEASE)
		{
			GM_CONFIG.giUpscalingKeyPressed = false;
		}

		//Toggle automatic revoxelization
		if(glfwGetKey(window, GM_KEY_R) == GLFW_PRESS && !GM_CONFIG.autoVoxelizeKeyPressed)
		{
			GM_CONFIG.autoVoxelize = !GM_CONFIG.autoVoxelize;
			GM_CONFIG.autoVoxelizeKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_R) == GLFW_RELEASE)
		{
			GM_CONFIG.autoVoxelizeKeyPressed = false;
		}

		//Enable/disable normal mapping
		if(glfwGetKey(window, GM_KEY_N) == GLFW_PRESS && !GM_CONFIG.normalMappingKeyPressed)
		{
			GM_CONFIG.normalMapping = !GM_CONFIG.normalMapping;
			GM_CONFIG.normalMappingKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_N) == GLFW_RELEASE)
		{
			GM_CONFIG.normalMappingKeyPressed = false;
		}

		//Adjust film speed
		if(glfwGetKey(window, GM_KEY_Q) == GLFW_PRESS)
		{
			if(m_Camera.GetSensitivity() > 5.0f)
				m_Camera.SetSensitivity(m_Camera.GetSensitivity() - 5.0f);
			else
				m_Camera.SetSensitivity(0.0f);
		}
		else if(glfwGetKey(window, GM_KEY_E) == GLFW_PRESS)
		{
			m_Camera.SetSensitivity(m_Camera.GetSensitivity() + 5.0f);
		}

		//Enable/disable wireframe rendering
		if(glfwGetKey(window, GM_KEY_Z) == GLFW_PRESS && !GM_CONFIG.wireframeKeyPressed)
		{
			GM_CONFIG.wireframe = !GM_CONFIG.wireframe;
			GM_CONFIG.wireframeKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_Z) == GLFW_RELEASE)
			GM_CONFIG.wireframeKeyPressed = false;

		//Enable/disable temporal anti-aliasing
		if(glfwGetKey(window, GM_KEY_T) == GLFW_PRESS && !GM_CONFIG.taaKeyPressed)
		{
			GM_CONFIG.taa = !GM_CONFIG.taa;
			GM_CONFIG.taaKeyPressed = true;
		}
		if(glfwGetKey(window, GM_KEY_T) == GLFW_RELEASE)
		{
			GM_CONFIG.taaKeyPressed = false;
		}

		//Set render mode
		if(Input::IsKeyPressed(GM_KEY_0))
			GM_CONFIG.renderMode = 0;
		if(Input::IsKeyPressed(GM_KEY_1))
			GM_CONFIG.renderMode = 1;
		if(Input::IsKeyPressed(GM_KEY_2))
			GM_CONFIG.renderMode = 2;
		if(Input::IsKeyPressed(GM_KEY_3))
			GM_CONFIG.renderMode = 3;
		if(Input::IsKeyPressed(GM_KEY_4))
			GM_CONFIG.renderMode = 4;
		if(Input::IsKeyPressed(GM_KEY_5))
			GM_CONFIG.renderMode = 5;
		if(Input::IsKeyPressed(GM_KEY_6))
			GM_CONFIG.renderMode = 6;
		if(Input::IsKeyPressed(GM_KEY_7))
			GM_CONFIG.renderMode = 7;
		if(Input::IsKeyPressed(GM_KEY_8))
			GM_CONFIG.renderMode = 8;
	}

	void RenderingSystem::Render()
	{
		m_Camera.Update();

		std::vector<std::pair<EntityID, float>> persistingEntities;
		FrustumCull(persistingEntities);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//Set render mode to wireframe if enabled
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Geometry pass
		glViewport(0, 0, m_RenderResolutionWidth, m_RenderResolutionHeight);
		GM_RENDERING_CONTEXT.EnableDepthTesting();

		RenderSurface &geometryBuffer = GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_G_Buffer);
		geometryBuffer.Bind();
		geometryBuffer.Clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader &geometryBufferShader = m_ShaderManager.Get(m_G_BufferShader);
		geometryBufferShader.Bind();
		geometryBufferShader.UploadUniform("VP",                     m_Camera.GetViewProjectionMatrix());
		geometryBufferShader.UploadUniform("previousVP",             m_Camera.GetPreviousViewProjectionMatrix());
		geometryBufferShader.UploadUniform("temporalJitter",         glm::vec2(0.0f));
		geometryBufferShader.UploadUniform("previousTemporalJitter", glm::vec2(0.0f));
		geometryBufferShader.UploadUniform("normalMapping",          GM_CONFIG.normalMapping);

		for(const std::pair<EntityID, float> &i : persistingEntities)
		{
			RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i.first);

			//Generate render commands
			auto GenerateRenderCommandKey = [](const uint32_t materialIndex, const float depth){
				//Upper 20 bits: material | Lower 12 bits: depth
				//IEEE floats keep their relative order when interpreted as integers (assuming sign bit is equal across all values)
				const uint32_t materialIndexBitShift = 20;
				return ((uint32_t)depth & 0xfff) | ((materialIndex << materialIndexBitShift) & 0xfffff);
			};

			//auto packet = m_G_BufferBucket.AddRenderCommand<RenderCommand::RenderIndexed>(GenerateRenderCommandKey(renderableComponent->material, i.second));

			geometryBufferShader.UploadUniform("M",         renderableComponent->modelMatrix);
			geometryBufferShader.UploadUniform("previousM", renderableComponent->previousModelMatrix);
			
			PBR_Material &material = m_Materials[renderableComponent->materialIndex];
			GM_RENDERING_CONTEXT.GetTexture2Ds().Get(material.albedo).Bind(0);
			GM_RENDERING_CONTEXT.GetTexture2Ds().Get(material.normal).Bind(1);
			GM_RENDERING_CONTEXT.GetTexture2Ds().Get(material.roughness).Bind(2);
			GM_RENDERING_CONTEXT.GetTexture2Ds().Get(material.metalness).Bind(3);
			GM_RENDERING_CONTEXT.GetTexture2Ds().Get(material.emissivity).Bind(4);

			IndexBuffer       &indexBuffer       = GM_RENDERING_CONTEXT.GetIndexBuffers().Get(renderableComponent->indexBuffer);
			VertexArrayBuffer &vertexArrayBuffer = GM_RENDERING_CONTEXT.GetVertexArrayBuffers().Get(renderableComponent->vertexArrayBuffer);

			vertexArrayBuffer.Bind();
			GM_RENDERING_CONTEXT.RenderIndexed(indexBuffer.GetIndexCount());
		}

		//m_G_BufferBucket.SortPackets();
		//m_G_BufferBucket.DispatchPackets(GM_RENDERING_CONTEXT);

		GM_RENDERING_CONTEXT.DisableDepthTesting();

		//Reset render mode back to fill
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Optimization: change internal format of separable circular blur convolution textures from floating point

	//Deferred shading
		RenderSurface &finalBuffer = GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_FinalBuffer);
		finalBuffer.Bind();
		finalBuffer.Clear();

		Shader &deferredLightingShader = m_ShaderManager.Get(m_DeferredLightingShader);
		deferredLightingShader.Bind();

		GM_LOG_CORE_INFO("Point lights: %d", m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities.size());

		uint32_t pointLightIndex = 0;
		for(const auto i : m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities)
		{
			PointLightComponent *pointLightComponent = m_World->GetComponent<PointLightComponent>(i);

			std::string positionUniformName = "pointLights[" + std::to_string(pointLightIndex) + "].position";
			std::string radianceUniformName = "pointLights[" + std::to_string(pointLightIndex) + "].radiance";

			deferredLightingShader.UploadUniform(positionUniformName, pointLightComponent->position);
			deferredLightingShader.UploadUniform(radianceUniformName, pointLightComponent->radiance);
			//TODO: Fix coneAperture uniform name
			deferredLightingShader.UploadUniform("pointLights[0].coneAperture", 0.0f);
			pointLightIndex++;
		}

		deferredLightingShader.UploadUniform("numLights",            (int)m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities.size());
		deferredLightingShader.UploadUniform("cameraPos",            m_Camera.GetPosition());
		deferredLightingShader.UploadUniform("voxelGridSize",        GM_CONFIG.voxelGridSize);
		deferredLightingShader.UploadUniform("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		deferredLightingShader.UploadUniform("voxelWorldSize",       GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		deferredLightingShader.UploadUniform("voxelGridPos",         GM_CONFIG.voxelGridPos);

		deferredLightingShader.UploadUniform("renderMode",           GM_CONFIG.renderMode);
		deferredLightingShader.UploadUniform("debug2",               GM_CONFIG.debug2);

		GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_G_BufferPositionMetalness).Bind(0);
		GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_G_BufferNormal).Bind(1);
		GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_G_BufferAlbedoEmissivityRoughness).Bind(2);
		m_BRDF_LUT.Bind(3);

		RenderFullscreenWindow();
		
		//Copy gBuffer depth to HDR FBO
		finalBuffer.BlitDepthBuffer(geometryBuffer, m_RenderResolutionWidth, m_RenderResolutionHeight, TextureInterpolationMode::Point);
		finalBuffer.Bind();

		/*
		//Forward rendering
		//glEnable(GL_DEPTH_TEST);

		//Lights
		m_ShaderManager->Get(m_LightShader).Bind();
		m_ShaderManager->Get(m_LightShader).UploadUniformMat4("projection", projectionMatrix);
		m_ShaderManager->Get(m_LightShader).UploadUniformMat4("view",       viewMatrix);
		//Light 0
		m_ShaderManager->Get(m_LightShader).UploadUniformMat4("M",          glm::mat4());
		m_ShaderManager->Get(m_LightShader).UploadUniformVec3("lightColor", pointLight0.GetColor());
		
		RenderFullscreenQuad();
		
			//Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Bind();
		glm::mat4 cubemapView(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.UploadUniformMat4("projection", projectionMatrix);
		skyboxShader.UploadUniformMat4("view", cubemapView);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glBindVertexArray(skyboxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glDisable(GL_DEPTH_TEST);
		*/

		//Post-process (tonemapping, exposure, film grain, gamma correction)
		glViewport(0, 0, Application::GetInstance().GetWindow().GetWidth(), Application::GetInstance().GetWindow().GetHeight());
		RenderSurface::BindBackBuffer();
		RenderSurface::ClearBackBuffer();
		
		Shader &postProcessShader = m_ShaderManager.Get(m_PostprocessShader);
		postProcessShader.Bind();
		postProcessShader.UploadUniform("exposureNormalizationCoeffecient", m_Camera.GetExposure());
		postProcessShader.UploadUniform("time", (float)glfwGetTime());

		GM_RENDERING_CONTEXT.GetTexture2Ds().Get(m_FinalImage).Bind(0);

		RenderFullscreenWindow();
	}

	void RenderingSystem::FrustumCull(std::vector<std::pair<EntityID, float>> &persistingEntities) const
	{
		for(const auto i : m_EntityGroups[GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX].entities)
		{
			BoundingVolumeComponent *boundingVolumeComponent = m_World->GetComponent<BoundingVolumeComponent>(i);
			//Bounding sphere intersection
			if(m_Camera.GetFrustum().Intersects(boundingVolumeComponent->worldSpaceBoundingSphere))
			{
				//AABB intersection
				//if(cameraFrustum.Intersects(renderableComponent->worldSpaceAxisAlignedBoundingBox))
				{
					float depth = glm::length(m_Camera.GetPosition() - boundingVolumeComponent->worldSpaceBoundingSphere.position);
					persistingEntities.emplace_back(std::make_pair(i, depth));
				}
			}
		}

		GM_LOG_CORE_TRACE("Renderable entity count before culling: %d | Renderable entity count after culling: %d", m_EntityGroups[GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX].entities.size(), persistingEntities.size());
		GM_ASSERT(persistingEntities.size() > 0, "No persisting entities after culling")
	}

	void RenderingSystem::Shutdown()
	{}

	void RenderingSystem::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(GM_BIND_EVENT_CALLBACK(RenderingSystem::OnWindowResize));
	}

	bool RenderingSystem::OnWindowResize(WindowResizeEvent &event)
	{
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return true;
	}
}