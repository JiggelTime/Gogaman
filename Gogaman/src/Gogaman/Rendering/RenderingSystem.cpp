#include "pch.h"
#include "RenderingSystem.h"

#include "Gogaman/Base.h"
#include "Gogaman/Application.h"
#include "Gogaman/Logging/Log.h"

#include "Gogaman/Core/Time.h"

#include "Gogaman/ECS/World.h"
#include "RenderableComponent.h"
#include "LightComponent.h"

#include "Gogaman/Input.h"

#include "Gogaman/Events/EventDispatcher.h"

#define GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX        0
#define GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX       1
#define GM_RENDERING_SYSTEM_DIRECTIONAL_LIGHT_GROUP_INDEX 2

#if GM_RENDERING_API == GM_RENDERING_API_OPENGL
	#define GM_SHADERS_DIRECTORY std::string("../Gogaman/src/Platform/OpenGL/Shaders/")
#else
	#error
#endif

#define GM_BRDF_LUT_XY 512

namespace Gogaman
{
	RenderingSystem::RenderingSystem()
	{
		EntityGroup renderableGroup;
		renderableGroup.componentFlags.set(GetComponentTypeID<RenderableComponent>());
		AddEntityGroup(std::move(renderableGroup));

		EntityGroup pointLightGroup;
		pointLightGroup.componentFlags.set(GetComponentTypeID<PointLightComponent>());
		AddEntityGroup(std::move(pointLightGroup));

		EntityGroup directionalLightGroup;
		directionalLightGroup.componentFlags.set(GetComponentTypeID<DirectionalLightComponent>());
		AddEntityGroup(std::move(directionalLightGroup));
	}

	void RenderingSystem::Initialize()
	{
		m_RenderResolutionWidth  = static_cast<int>(Application::GetInstance().GetWindow().GetWidth()  * GM_CONFIG.resScale);
		m_RenderResolutionHeight = static_cast<int>(Application::GetInstance().GetWindow().GetHeight() * GM_CONFIG.resScale);

		InitializeRenderSurfaces();

		m_ShaderManager = std::make_unique<ShaderManager>();
		InitializeShaders();

		m_FullscreenTriangle = std::make_unique<FullscreenTriangle>();

		//Generate BRDF LUT for split-sum approximation
		glViewport(0, 0, GM_BRDF_LUT_XY, GM_BRDF_LUT_XY);
		m_BRDF_Buffer->Bind();
		m_ShaderManager->Get(m_PrecomputeBRDFShader).Bind();
		RenderFullscreenWindow();
	}

	void RenderingSystem::InitializeRenderSurfaces()
	{
		//Precomputed BRDF
		m_BRDF_Buffer = std::make_unique<RenderSurface>();

		m_BRDF_LUT.internalFormat    = TextureInternalFormat::XY16F;
		m_BRDF_LUT.format            = TextureFormat::XY;
		m_BRDF_LUT.interpolationMode = TextureInterpolationMode::Bilinear;
		m_BRDF_LUT.Generate(GM_BRDF_LUT_XY, GM_BRDF_LUT_XY);
		m_BRDF_Buffer->AddColorBuffer(m_BRDF_LUT);
		
		//G-Buffer
		m_G_Buffer = std::make_unique<RenderSurface>();

		m_Texture2Ds["gPositionMetalness"].internalFormat    = TextureInternalFormat::XYZW16F;
		m_Texture2Ds["gPositionMetalness"].format            = TextureFormat::XYZW;
		m_Texture2Ds["gPositionMetalness"].interpolationMode = TextureInterpolationMode::Point;
		m_Texture2Ds["gPositionMetalness"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_G_Buffer->AddColorBuffer(m_Texture2Ds["gPositionMetalness"]);

		m_Texture2Ds["gNormal"].internalFormat    = TextureInternalFormat::XY16F;
		m_Texture2Ds["gNormal"].format            = TextureFormat::XY;
		m_Texture2Ds["gNormal"].interpolationMode = TextureInterpolationMode::Point;
		m_Texture2Ds["gNormal"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_G_Buffer->AddColorBuffer(m_Texture2Ds["gNormal"]);

		m_Texture2Ds["gAlbedoEmissivityRoughness"].internalFormat    = TextureInternalFormat::XYZW8;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].format            = TextureFormat::XYZW;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].interpolationMode = TextureInterpolationMode::Bilinear;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_G_Buffer->AddColorBuffer(m_Texture2Ds["gAlbedoEmissivityRoughness"]);

		m_Texture2Ds["gVelocity"].internalFormat    = TextureInternalFormat::XY16F;
		m_Texture2Ds["gVelocity"].format            = TextureFormat::XY;
		m_Texture2Ds["gVelocity"].interpolationMode = TextureInterpolationMode::Point;
		m_Texture2Ds["gVelocity"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_G_Buffer->AddColorBuffer(m_Texture2Ds["gVelocity"]);

		m_Texture2Ds["gDepth"].internalFormat    = TextureInternalFormat::Depth32;
		m_Texture2Ds["gDepth"].format            = TextureFormat::X;
		m_Texture2Ds["gDepth"].interpolationMode = TextureInterpolationMode::Point;
		m_Texture2Ds["gDepth"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_G_Buffer->AddDepthBuffer(m_Texture2Ds["gDepth"]);

		//Final image
		m_FinalBuffer = std::make_unique<RenderSurface>();

		m_Texture2Ds["finalImage"].internalFormat    = TextureInternalFormat::XYZW16F;
		m_Texture2Ds["finalImage"].format            = TextureFormat::XYZW;
		m_Texture2Ds["finalImage"].interpolationMode = TextureInterpolationMode::Bilinear;
		m_Texture2Ds["finalImage"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_FinalBuffer->AddColorBuffer(m_Texture2Ds["finalImage"]);

		m_Renderbuffers["finalImageDepth"].internalFormat = TextureInternalFormat::Depth32;
		m_Renderbuffers["finalImageDepth"].Generate(m_RenderResolutionWidth, m_RenderResolutionHeight);
		m_FinalBuffer->AddDepthBuffer(m_Renderbuffers["finalImageDepth"]);
	}

	void RenderingSystem::InitializeShaders()
	{
		m_PrecomputeBRDFShader   = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("precomputeBRDF.vs"), GM_SHADERS_DIRECTORY.append("precomputeBRDF.fs"));
		m_GBufferShader          = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("gbuffershader.vs"),  GM_SHADERS_DIRECTORY.append("gbuffershader.fs"));
		m_DeferredLightingShader = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("directPBR.vs"),      GM_SHADERS_DIRECTORY.append("directPBR.fs"));
		m_SkyboxShader           = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("skyboxshader.vs"),   GM_SHADERS_DIRECTORY.append("skyboxshader.fs"));
		m_LightShader            = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("lampshader.vs"),     GM_SHADERS_DIRECTORY.append("lampshader.fs"));
		m_PostprocessShader      = m_ShaderManager->Create(GM_SHADERS_DIRECTORY.append("postprocess.vs"),    GM_SHADERS_DIRECTORY.append("postprocess.fs"));

		//Upload uniform data
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("materialAlbedo",     0);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("materialNormal",     1);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("materialRoughness",  2);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("materialMetalness",  3);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("materialEmissivity", 4);

		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("gPositionMetalness",         0);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("gNormal",                    1);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("gAlbedoEmissivityRoughness", 2);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("BRDF_LUT",                   3);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("coneTracedDiffuse",          4);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("coneTracedSpecular",         5);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("voxelTexture",               6);

		m_ShaderManager->Get(m_SkyboxShader).UploadUniform("skybox", 0);

		m_ShaderManager->Get(m_PostprocessShader).UploadUniform("hdrTexture", 0);
	}

	void RenderingSystem::RenderFullscreenWindow() const
	{
		m_FullscreenTriangle->GetVertexArrayBuffer().Bind();
		Application::GetInstance().GetWindow().GetRenderingContext().RenderIndexed(m_FullscreenTriangle->GetIndexBuffer().GetNumIndices());
	}

	void RenderingSystem::Update()
	{
		frameCounter++;
		
		//Render resolution
		m_RenderResolutionWidth  = static_cast<int>(Application::GetInstance().GetWindow().GetWidth()  * GM_CONFIG.resScale);
		m_RenderResolutionHeight = static_cast<int>(Application::GetInstance().GetWindow().GetHeight() * GM_CONFIG.resScale);

		GLFWwindow *window = static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow());

		//Close window
		if(Input::IsKeyPressed(GM_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		//Reload shaders
		if(Input::IsKeyPressed(GM_KEY_R))
		{
			m_ShaderManager->ReloadAll();
			InitializeShaders();
		}

		//Movement
		if(Input::IsKeyPressed(GM_KEY_W))
			camera.ProcessKeyboardInput(FORWARD,  Time::GetDeltaTime());
		if(Input::IsKeyPressed(GM_KEY_S))
			camera.ProcessKeyboardInput(BACKWARD, Time::GetDeltaTime());
		if(Input::IsKeyPressed(GM_KEY_A))
			camera.ProcessKeyboardInput(LEFT,     Time::GetDeltaTime());
		if(Input::IsKeyPressed(GM_KEY_D))
			camera.ProcessKeyboardInput(RIGHT,    Time::GetDeltaTime());
		
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

		//Adjust exposure
		if(glfwGetKey(window, GM_KEY_Q) == GLFW_PRESS)
		{
			if(exposure > 0.01f)
				exposure -= 0.01f;
			else
				exposure = 0.0f;
		}
		else if(glfwGetKey(window, GM_KEY_E) == GLFW_PRESS)
		{
			exposure += 0.01f;
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
		//Update camera matrices
		previousViewProjectionMatrix = viewProjectionMatrix;
		projectionMatrix             = glm::perspective(glm::radians(camera.Zoom), aspectRatio, cameraNearPlane, cameraFarPlane);
		viewMatrix                   = camera.GetViewMatrix();
		viewProjectionMatrix         = projectionMatrix * viewMatrix;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//Set render mode to wireframe if enabled
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Geometry pass
		//TODO: G-Buffer and post processing should be done in low level renderer
		glViewport(0, 0, m_RenderResolutionWidth, m_RenderResolutionHeight);
		glEnable(GL_DEPTH_TEST);
		m_G_Buffer->Bind();
		m_G_Buffer->Clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_ShaderManager->Get(m_GBufferShader).Bind();
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("VP",                     viewProjectionMatrix);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("previousVP",             previousViewProjectionMatrix);
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("temporalJitter",         glm::vec2(0.0f));
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("previousTemporalJitter", glm::vec2(0.0f));
		m_ShaderManager->Get(m_GBufferShader).UploadUniform("normalMapping",          GM_CONFIG.normalMapping);

		//Generate render data from renderable entities
		//TODO: Store distance to camera of each mesh for sorting by depth
		for(auto i : m_EntityGroups[GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX].entities)
		{
			//Is the entire renderable component needed to be loaded into cache here?
			RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i);

			//Frustrum cull...
			
			//Generate render commands...
			//If this command's state is different from the previous, generate a state change command

			auto GenerateRenderCommandKey = [](const uint32_t materialIndex, const float depth){
				const uint32_t materialIndexBitShift = 20;

				//Upper 20 bits = materialIndex
				//Holds 1048576 unique material indices
				//Lower 12 bits = depth
				//Holds 4096 unique depth values

				//IEEE floats keep their relative order when interpreted as integers (assuming sign bit is equal across all values)
				return ((uint32_t)depth & 0xfff) | ((materialIndex << materialIndexBitShift) & 0xfffff);
			};

			m_ShaderManager->Get(m_GBufferShader).UploadUniform("M",         renderableComponent->modelMatrix);
			m_ShaderManager->Get(m_GBufferShader).UploadUniform("previousM", renderableComponent->modelMatrixHistory);
			
			renderableComponent->material.BindTextures();

			renderableComponent->vertexArrayBuffer->Bind();
			Application::GetInstance().GetWindow().GetRenderingContext().RenderIndexed(renderableComponent->indexBuffer->GetNumIndices());
		}

		//Sort render batches (using flags)...
		
		//Render the command queue...

		glDisable(GL_DEPTH_TEST);

		//Reset render mode back to fill
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Optimization: change internal format of separable circular blur convolution textures from floating point

	//Deferred shading
		m_FinalBuffer->Bind();
		m_FinalBuffer->Clear();

		m_ShaderManager->Get(m_DeferredLightingShader).Bind();

		uint32_t pointLightIndex = 0;
		for(auto i : m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities)
		{
			PointLightComponent *pointLightComponent = m_World->GetComponent<PointLightComponent>(i);

			std::string positionUniformName = "pointLights[" + std::to_string(pointLightIndex) + "].position";
			std::string radianceUniformName = "pointLights[" + std::to_string(pointLightIndex) + "].radiance";

			m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform(positionUniformName, pointLightComponent->position);
			m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform(radianceUniformName, pointLightComponent->radiance);
			m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("pointLights[0].coneAperture", 0.0f);
			pointLightIndex++;
		}

		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("numLights",            (int)m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities.size());
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("cameraPos",            camera.Position);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("voxelGridSize",        GM_CONFIG.voxelGridSize);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("voxelWorldSize",       GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("voxelGridPos",         GM_CONFIG.voxelGridPos);

		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("renderMode",           GM_CONFIG.renderMode);
		m_ShaderManager->Get(m_DeferredLightingShader).UploadUniform("debug2",               GM_CONFIG.debug2);

		m_Texture2Ds["gPositionMetalness"].Bind(0);
		m_Texture2Ds["gNormal"].Bind(1);
		m_Texture2Ds["gAlbedoEmissivityRoughness"].Bind(2);
		m_BRDF_LUT.Bind(3);

		RenderFullscreenWindow();
		
		//Copy gBuffer depth to HDR FBO
		m_FinalBuffer->BlitDepthBuffer(*m_G_Buffer.get(), m_RenderResolutionWidth, m_RenderResolutionHeight, TextureInterpolationMode::Point);
		m_FinalBuffer->Bind();

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

		m_ShaderManager->Get(m_PostprocessShader).Bind();
		m_ShaderManager->Get(m_PostprocessShader).UploadUniform("exposureBias", exposure);
		m_ShaderManager->Get(m_PostprocessShader).UploadUniform("time",         (float)glfwGetTime());

		m_Texture2Ds["finalImage"].Bind(0);

		RenderFullscreenWindow();

		if(firstIteration)
			firstIteration = false;
	}

	void RenderingSystem::Shutdown()
	{}

	void RenderingSystem::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(GM_BIND_EVENT_CALLBACK(RenderingSystem::OnWindowResize));

		dispatcher.Dispatch<MouseMoveEvent>(GM_BIND_EVENT_CALLBACK(RenderingSystem::OnMouseMove));
		dispatcher.Dispatch<MouseScrollEvent>(GM_BIND_EVENT_CALLBACK(RenderingSystem::OnMouseScroll));
	}

	bool RenderingSystem::OnWindowResize(WindowResizeEvent &event)
	{
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return true;
	}

	bool RenderingSystem::OnMouseMove(MouseMoveEvent &event)
	{
		if(firstMouse)
		{
			lastX = event.GetPositionX();
			lastY = event.GetPositionY();
			firstMouse = false;
		}

		float xOffset = event.GetPositionX() - lastX;
		//Y axis is reversed
		float yOffset = lastY - event.GetPositionY();
		camera.ProcessMouseInput(xOffset, yOffset);

		lastX = event.GetPositionX();
		lastY = event.GetPositionY();
		return true;
	}

	bool RenderingSystem::OnMouseScroll(MouseScrollEvent &event)
	{
		camera.ProcessMouseScrollInput(event.GetOffsetY());
		return true;
	}
}