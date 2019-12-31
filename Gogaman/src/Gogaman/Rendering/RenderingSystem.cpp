#include "pch.h"
#include "RenderingSystem.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Time.h"
#include "Gogaman/Core/Application.h"

#include "Gogaman/Input/Input.h"

#include "Gogaman/Events/EventDispatcher.h"

#include "Gogaman/ECS/Entity.h"
#include "Gogaman/ECS/World.h"

#include "Gogaman/RenderHardwareInterface/DeviceMemory.h"
#include "Gogaman/RenderHardwareInterface/Sampler.h"
#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Buffer.h"
#include "Gogaman/RenderHardwareInterface/Shader.h"
#include "Gogaman/RenderHardwareInterface/ShaderProgram.h"
#include "Gogaman/RenderHardwareInterface/VertexLayout.h"
#include "Gogaman/RenderHardwareInterface/RenderSurface.h"
#include "Gogaman/RenderHardwareInterface/TransferCommandRecorder.h"
#include "Gogaman/RenderHardwareInterface/RenderCommandRecorder.h"

#include "Gogaman/SpatialComponent.h"
#include "Gogaman/BoundingVolumeComponent.h"
#include "RenderableComponent.h"
#include "LightComponent.h"

#include <FlexData.h>

#define GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX        0
#define GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX       1
#define GM_RENDERING_SYSTEM_DIRECTIONAL_LIGHT_GROUP_INDEX 2

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
		using namespace RHI;

		m_RenderResolutionWidth  = (uint16_t)(Application::GetInstance().GetWindow().GetWidth()  * GM_CONFIG.resScale);
		m_RenderResolutionHeight = (uint16_t)(Application::GetInstance().GetWindow().GetHeight() * GM_CONFIG.resScale);

		m_Camera.SetFocalLength(20.0f);
		m_Camera.SetSensorHeight(24.0f);
		m_Camera.SetAspectRatio(GM_CONFIG.aspectRatio);

		CreateRenderSurfaces();
		CreateShaders();

		const VertexLayout vertexLayout(
		{
			//Position
			{ Shader::DataType::Float3 },
			//UV
			{ Shader::DataType::Float2 },
			//Normal
			{ Shader::DataType::Float3 },
			//Tangent
			{ Shader::DataType::Float3 }
		});

		m_DescriptorGroupLayouts.reserve(3);
		//Frame data
		DescriptorGroupLayout::Binding *frameDataDescriptorBindings = new DescriptorGroupLayout::Binding[3];
		frameDataDescriptorBindings[0].type = DescriptorHeap::Type::ShaderConstantBuffer;
		frameDataDescriptorBindings[1].type = DescriptorHeap::Type::Sampler;
		frameDataDescriptorBindings[2].type = DescriptorHeap::Type::Sampler;
		m_DescriptorGroupLayouts.emplace_back(3, frameDataDescriptorBindings, Shader::StageFlags::All);
		//Material data
		DescriptorGroupLayout::Binding *materialDataDescriptorBindings = new DescriptorGroupLayout::Binding[5];
		materialDataDescriptorBindings[0].type = DescriptorHeap::Type::ShaderTexture;
		materialDataDescriptorBindings[1].type = DescriptorHeap::Type::ShaderTexture;
		materialDataDescriptorBindings[2].type = DescriptorHeap::Type::ShaderTexture;
		materialDataDescriptorBindings[3].type = DescriptorHeap::Type::ShaderTexture;
		materialDataDescriptorBindings[4].type = DescriptorHeap::Type::ShaderTexture;
		m_DescriptorGroupLayouts.emplace_back(5, materialDataDescriptorBindings, Shader::StageFlags::Pixel);
		//Mesh data
		DescriptorGroupLayout::Binding *meshDataDescriptorBinding = new DescriptorGroupLayout::Binding;
		meshDataDescriptorBinding->type = DescriptorHeap::Type::ShaderConstantBuffer;
		m_DescriptorGroupLayouts.emplace_back(1, meshDataDescriptorBinding, Shader::StageFlags::Vertex);

		RenderState::DepthStencilState depthStencilState;
		depthStencilState.depthComparisonOperation = RenderState::ComparisonOperation::Less;
		depthStencilState.isDepthTestEnabled       = true;
		depthStencilState.isDepthWriteEnabled      = true;

		RenderState::BlendState blendState;

		m_RenderStates.reserve(GM_SWAP_CHAIN_BUFFER_COUNT);
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			m_RenderStates.emplace_back(m_DescriptorGroupLayouts, vertexLayout, m_ShaderProgramID, g_Device->GetSwapChainRenderSurfaceIDs()[i], depthStencilState, blendState, m_RenderResolutionWidth, m_RenderResolutionHeight, RenderState::CullOperation::None);
		}

		DescriptorHeap::DescriptorCounts descriptorCounts;
		descriptorCounts.SetDescriptorCount<DescriptorHeap::Type::ShaderConstantBuffer>(GM_SWAP_CHAIN_BUFFER_COUNT * 17);
		descriptorCounts.SetDescriptorCount<DescriptorHeap::Type::Sampler>(GM_SWAP_CHAIN_BUFFER_COUNT * 2);
		descriptorCounts.SetDescriptorCount<DescriptorHeap::Type::ShaderTexture>(GM_SWAP_CHAIN_BUFFER_COUNT * 5 * 16);
		m_DescriptorHeap = std::make_unique<DescriptorHeap>(std::move(descriptorCounts));

		m_RenderCommandHeap = std::make_unique<CommandHeap>(CommandHeap::Type::Render);
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			m_CommandBuffers[i] = m_RenderCommandHeap->CreateCommandBuffer();

			m_FrameDataDescriptorGroups[i] = std::make_unique<DescriptorGroup>(m_DescriptorHeap.get(), &m_DescriptorGroupLayouts[0]);
			
			m_FrameDataBuffers[i] = g_Device->GetResources().buffers.Create(DeviceMemory::Type::DeviceUpload, sizeof(FrameData), Buffer::BindFlags::ShaderConstants);
		}

		m_PointSampler       = g_Device->GetResources().samplers.Create(Sampler::Interpolation::Point);
		m_AnisotropicSampler = g_Device->GetResources().samplers.Create(Sampler::Interpolation::Anisotropic);

		ImportFlexData();
	}

	void RenderingSystem::CreateRenderSurfaces()
	{
		using namespace RHI;
	}

	void RenderingSystem::CreateShaders()
	{
		using namespace RHI;
		
		auto LoadShader = [](const char *filepath)
		{
			FILE *file = fopen(filepath, "rb");
			fseek(file, 0, SEEK_END);
			uint32_t size = ftell(file);
			fseek(file, 0, SEEK_SET);
			uint8_t *buffer = new uint8_t[size];
			fread(buffer, size, 1, file);
			fclose(file);

			return std::pair<uint32_t, uint8_t *>(size, buffer);
		};

		auto vertexShaderData = LoadShader("D:/dev/Gogaman/Gogaman/Shaders/vert.spv");
		m_VertexShaderID = g_Device->GetResources().shaders.Create(vertexShaderData.first, vertexShaderData.second);

		auto pixelShaderData = LoadShader("D:/dev/Gogaman/Gogaman/Shaders/frag.spv");
		m_PixelShaderID = g_Device->GetResources().shaders.Create(pixelShaderData.first, pixelShaderData.second);
		 
		auto &shaderProgram = g_Device->GetResources().shaderPrograms.Create(m_ShaderProgramID);
		shaderProgram.SetShader<Shader::Stage::Vertex>(m_VertexShaderID);
		shaderProgram.SetShader<Shader::Stage::Pixel>(m_PixelShaderID);
	}

	void RenderingSystem::RecreateShaders()
	{
		g_Device->GetResources().shaders.Destroy(m_VertexShaderID);
		g_Device->GetResources().shaders.Destroy(m_PixelShaderID);

		g_Device->GetResources().shaderPrograms.Destroy(m_ShaderProgramID);

		CreateShaders();
	}

	//TODO: OnEvent FlexDataImportEvent
	void RenderingSystem::ImportFlexData()
	{
		using namespace RHI;

		World &world = Application::GetInstance().GetWorld();
		
		FlexData::FlexData flexData;
		if(!FlexData::Import(flexData, "D:/dev/TestAssetGroup/TestData.flex"))
			GM_ASSERT(false, "Failed to import FlexData");

		FlexData::Print(flexData);

		m_TransferCommandHeap = std::make_unique<CommandHeap>(CommandHeap::Type::Transfer);
		std::unique_ptr<CommandBuffer> transferCommandBuffer = m_TransferCommandHeap->CreateCommandBuffer();
		TransferCommandRecorder        transferCommandRecorder(transferCommandBuffer.get());

		//Import materials
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			m_MaterialDataDescriptorGroups[i].reserve(1);
		}

		m_Materials.reserve(1);
		PBR_Material &material = m_Materials.emplace_back();

		auto ImportFlexTexture = [&](const Texture::Format format, const FlexData::FlexTextureData &flexTexture)
		{
			TextureID textureID;
			Texture &texture = g_Device->GetResources().textures.Create(textureID, format, flexTexture.width, flexTexture.height);

			BufferID stagingBufferID;
			Buffer &stagingBuffer = g_Device->GetResources().buffers.Create(stagingBufferID, DeviceMemory::Type::DeviceUpload, Texture::GetFormatDataSize(format) * flexTexture.width * flexTexture.height);
			stagingBuffer.SetData(flexTexture.data);

			transferCommandRecorder.CopyData(stagingBuffer, texture);

			return textureID;
		};

		material.albedo     = ImportFlexTexture(Texture::Format::RGBW8, flexData.albedoTextures[0]);
		material.normal     = ImportFlexTexture(Texture::Format::XYZW8, flexData.normalTextures[0]);
		material.roughness  = ImportFlexTexture(Texture::Format::X8,    flexData.roughnessTextures[0]);
		material.metalness  = ImportFlexTexture(Texture::Format::X8,    flexData.metalnessTextures[0]);
		material.emissivity = ImportFlexTexture(Texture::Format::X8,    flexData.emissivityTextures[0]);

		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			m_MaterialDataDescriptorGroups[i].emplace_back(m_DescriptorHeap.get(), &m_DescriptorGroupLayouts[1]);
		}

		//Import meshes
		for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			m_MeshDataDescriptorGroups[i].reserve(flexData.meshes.size());
		}

		for(const auto &i : flexData.meshes)
		{
			Entity meshEntity = world.CreateEntity();

			SpatialComponent spatialComponent;
			spatialComponent.position      = glm::vec3(i.transform.position[0], i.transform.position[1], i.transform.position[2]);
			//spatialComponent.rotation      = glm::vec3(i.transform.rotation[0], i.transform.rotation[1], i.transform.rotation[2]);
			spatialComponent.rotation      = glm::vec3(0.0f, 0.0f, 0.0f);
			spatialComponent.rotationAngle = 0.0f;
			spatialComponent.scale         = glm::vec3(i.transform.scale[0], i.transform.scale[1], i.transform.scale[2]);
			world.AddComponent<SpatialComponent>(meshEntity, std::move(spatialComponent));

			BoundingVolumeComponent boundingVolumeComponent;
			//Bounding sphere
			boundingVolumeComponent.boundingSphere.position = glm::vec3(i.boundingSphere.position[0], i.boundingSphere.position[1], i.boundingSphere.position[2]);
			boundingVolumeComponent.boundingSphere.radius   = i.boundingSphere.radius;
			//AABB
			boundingVolumeComponent.axisAlignedBoundingBox.minimum = glm::vec3(i.axisAlignedBoundingBox.minimum[0], i.axisAlignedBoundingBox.minimum[1], i.axisAlignedBoundingBox.minimum[2]);
			boundingVolumeComponent.axisAlignedBoundingBox.maximum = glm::vec3(i.axisAlignedBoundingBox.maximum[0], i.axisAlignedBoundingBox.maximum[1], i.axisAlignedBoundingBox.maximum[2]);
			world.AddComponent<BoundingVolumeComponent>(meshEntity, std::move(boundingVolumeComponent));

			RenderableComponent renderableComponent;

			//Material index
			renderableComponent.materialIndex = 0;

			//Vertex buffer
			Buffer &vertexBuffer = g_Device->GetResources().buffers.Create(renderableComponent.vertexBufferID, DeviceMemory::Type::Device, FLEX_VERTEX_DATA_SIZE * (uint32_t)i.vertexBuffer.size(), Buffer::BindFlags::Vertex);

			BufferID vertexStagingBufferID;
			Buffer &vertexStagingBuffer = g_Device->GetResources().buffers.Create(vertexStagingBufferID, DeviceMemory::Type::DeviceUpload, FLEX_VERTEX_DATA_SIZE * (uint32_t)i.vertexBuffer.size(), Buffer::BindFlags::Vertex);
			vertexStagingBuffer.SetData(i.vertexBuffer.data());

			transferCommandRecorder.CopyData(vertexStagingBuffer, vertexBuffer);

			//Index buffer
			Buffer &indexBuffer = g_Device->GetResources().buffers.Create(renderableComponent.indexBufferID, DeviceMemory::Type::Device, sizeof(uint16_t) * i.indexBuffer.size(), Buffer::BindFlags::Index);
			
			BufferID indexStagingBufferID;
			Buffer &indexStagingBuffer = g_Device->GetResources().buffers.Create(indexStagingBufferID, DeviceMemory::Type::DeviceUpload, sizeof(uint16_t) * i.indexBuffer.size(), Buffer::BindFlags::Index);
			indexStagingBuffer.SetData(i.indexBuffer.data());

			transferCommandRecorder.CopyData(indexStagingBuffer, indexBuffer);

			//Shader data buffer
			renderableComponent.shaderDataBufferID = g_Device->GetResources().buffers.Create(DeviceMemory::Type::DeviceUpload, sizeof(MeshData), Buffer::BindFlags::ShaderConstants);

			world.AddComponent(meshEntity, std::move(renderableComponent));

			for(uint8_t i = 0; i < GM_SWAP_CHAIN_BUFFER_COUNT; i++)
			{
				m_MeshDataDescriptorGroups[i].emplace_back(m_DescriptorHeap.get(), &m_DescriptorGroupLayouts[2]);
			}
		}

		transferCommandRecorder.StopRecording();
		g_Device->SubmitTransferCommands(1, transferCommandBuffer.get());

		//Import point lights
		for(const auto &i : flexData.pointLights)
		{
			Entity pointLightEntity = world.CreateEntity();

			PointLightComponent pointLightComponent;
			pointLightComponent.position = glm::vec3(i.position[0], i.position[1], i.position[2]);
			pointLightComponent.radiance = glm::vec3(i.radiance[0], i.radiance[1], i.radiance[2]);
			world.AddComponent(pointLightEntity, std::move(pointLightComponent));
		}

		//Import directional lights
		for(const auto &i : flexData.directionalLights)
		{
			Entity directionalLightEntity = world.CreateEntity();

			DirectionalLightComponent directionalLightComponent;
			directionalLightComponent.direction = glm::vec3(i.direction[0], i.direction[1], i.direction[2]);
			directionalLightComponent.radiance  = glm::vec3(i.radiance[0], i.radiance[1], i.radiance[2]);
			world.AddComponent(directionalLightEntity, std::move(directionalLightComponent));
		}
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
			RecreateShaders();
		}
		
		//Camera film speed
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
	}

	void RenderingSystem::Render()
	{
		using namespace RHI;

		m_Camera.Update();

		const auto swapChainImageIndex = g_Device->GetNativeData().vulkanSwapChainImageIndex;

		FrameData frameData;
		frameData.cameraViewMatrix           = m_Camera.GetViewMatrix();
		frameData.cameraProjectionMatrix     = m_Camera.GetProjectionMatrix();
		frameData.cameraViewProjectionMatrix = m_Camera.GetViewProjectionMatrix();
		g_Device->GetResources().buffers.Get(m_FrameDataBuffers[swapChainImageIndex]).SetData(&frameData);
		//Move to initialize?
		m_FrameDataDescriptorGroups[swapChainImageIndex]->SetShaderConstantBuffer(0, m_FrameDataBuffers[swapChainImageIndex]);
		m_FrameDataDescriptorGroups[swapChainImageIndex]->SetSampler(1, g_Device->GetResources().samplers.Get(m_PointSampler));
		m_FrameDataDescriptorGroups[swapChainImageIndex]->SetSampler(2, g_Device->GetResources().samplers.Get(m_AnisotropicSampler));

		RenderCommandRecorder commandRecorder(m_CommandBuffers[swapChainImageIndex].get(), &m_RenderStates[swapChainImageIndex]);
		commandRecorder.BindDescriptorGroup(0, *m_FrameDataDescriptorGroups[swapChainImageIndex].get());

		for(uint16_t i = 0; i < m_Materials.size(); i++)
		{
			const PBR_Material &material = m_Materials[i];
			//Move to flex data import, not needed @ runtime?
			m_MaterialDataDescriptorGroups[swapChainImageIndex][i].SetShaderTexture(0, g_Device->GetResources().textures.Get(material.albedo));
			m_MaterialDataDescriptorGroups[swapChainImageIndex][i].SetShaderTexture(1, g_Device->GetResources().textures.Get(material.normal));
			m_MaterialDataDescriptorGroups[swapChainImageIndex][i].SetShaderTexture(2, g_Device->GetResources().textures.Get(material.roughness));
			m_MaterialDataDescriptorGroups[swapChainImageIndex][i].SetShaderTexture(3, g_Device->GetResources().textures.Get(material.metalness));
			m_MaterialDataDescriptorGroups[swapChainImageIndex][i].SetShaderTexture(4, g_Device->GetResources().textures.Get(material.emissivity));
		}

		for(const EntityID i : m_EntityGroups[GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX].entities)
		{
			const RenderableComponent &renderableComponent = m_World->GetComponent<RenderableComponent>(i);

			commandRecorder.BindDescriptorGroup(1, m_MaterialDataDescriptorGroups[swapChainImageIndex][renderableComponent.materialIndex]);

			MeshData meshData;
			meshData.meshMatrix = renderableComponent.meshMatrix;
			g_Device->GetResources().buffers.Get(renderableComponent.shaderDataBufferID).SetData(&meshData);
			m_MeshDataDescriptorGroups[swapChainImageIndex][i].SetShaderConstantBuffer(0, renderableComponent.shaderDataBufferID);

			commandRecorder.BindDescriptorGroup(2, m_MeshDataDescriptorGroups[swapChainImageIndex][i]);

			commandRecorder.BindBuffer(0, g_Device->GetResources().buffers.Get(renderableComponent.vertexBufferID));
			commandRecorder.BindBuffer(0, g_Device->GetResources().buffers.Get(renderableComponent.indexBufferID));
			commandRecorder.IndexedRender(g_Device->GetResources().buffers.Get(renderableComponent.indexBufferID).GetSize() / sizeof(uint16_t), 0, 0);
		}

		commandRecorder.StopRecording();

		g_Device->SubmitRenderCommands(1, m_CommandBuffers[swapChainImageIndex].get());
		
		//GeometryPass();
		//DeferredShadingPass();
		
		//Copy gBuffer depth to HDR FBO
		//RenderSurface &finalBuffer = GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_FinalBuffer);
		//finalBuffer.CopyDepthBuffer(GM_RENDERING_CONTEXT.GetRenderSurfaces().Get(m_G_Buffer), m_RenderResolutionWidth, m_RenderResolutionHeight, TextureInterpolation::Point);

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

		//GM_RENDERING_CONTEXT.SetViewport(Application::GetInstance().GetWindow().GetWidth(), Application::GetInstance().GetWindow().GetHeight());
		
		//PostProcessPass();
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
		return true;
	}
}