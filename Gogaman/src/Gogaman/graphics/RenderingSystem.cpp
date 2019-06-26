#include "pch.h"
#include "RenderingSystem.h"

#include "Gogaman/Base.h"

#include "Gogaman/ECS/World.h"
#include "RenderableComponent.h"
#include "LightComponent.h"

#include "Gogaman/Application.h"
#include "Gogaman/Input.h"

#include "Gogaman/Events/EventDispatcher.h"

//#include "Gogaman/Graphics/Lights/Pointlight.h"

#define GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX        0
#define GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX       1
#define GM_RENDERING_SYSTEM_DIRECTIONAL_LIGHT_GROUP_INDEX 2

namespace Gogaman
{
	RenderingSystem::RenderingSystem()
	{
		//m_NumEntityGroups = 3;

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

		//Configure global OpenGL state
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_TEXTURE_3D);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		m_FullscreenTriangle = std::make_unique<FullscreenTriangle>();

		//Generate BRDF LUT for split-sum approximation
		glViewport(0, 0, 512, 512);
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
		m_BRDF_LUT.Generate(m_BRDF_LUT_Width, m_BRDF_LUT_Height);
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
		m_PrecomputeBRDFShader   = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.vs", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.fs");
		m_GBufferShader          = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/gbuffershader.vs",  "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.fs");
		m_DeferredLightingShader = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/directPBR.vs",      "D:/dev/Gogaman/Gogaman/shaders/directPBR.fs");
		m_SkyboxShader           = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/skyboxshader.vs",   "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.fs");
		m_LightShader            = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/lampshader.vs",     "D:/dev/Gogaman/Gogaman/shaders/lampshader.fs");
		m_PostprocessShader      = m_ShaderManager->Create("D:/dev/Gogaman/Gogaman/shaders/postprocess.vs",    "D:/dev/Gogaman/Gogaman/shaders/postprocess.fs");

		//Upload uniform data
		m_ShaderManager->Get(m_GBufferShader).SetUniformInt("materialAlbedo",     0);
		m_ShaderManager->Get(m_GBufferShader).SetUniformInt("materialNormal",     1);
		m_ShaderManager->Get(m_GBufferShader).SetUniformInt("materialRoughness",  2);
		m_ShaderManager->Get(m_GBufferShader).SetUniformInt("materialMetalness",  3);
		m_ShaderManager->Get(m_GBufferShader).SetUniformInt("materialEmissivity", 4);

		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("gPositionMetalness",         0);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("gNormal",                    1);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("gAlbedoEmissivityRoughness", 2);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("BRDF_LUT",                   3);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("coneTracedDiffuse",          4);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("coneTracedSpecular",         5);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("voxelTexture",               6);

		m_ShaderManager->Get(m_SkyboxShader).SetUniformInt("skybox", 0);

		m_ShaderManager->Get(m_PostprocessShader).SetUniformInt("hdrTexture", 0);
	}

	void RenderingSystem::Update()
	{
		//Timing
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
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
			camera.ProcessKeyboardInput(FORWARD,  deltaTime);
		if(Input::IsKeyPressed(GM_KEY_S))
			camera.ProcessKeyboardInput(BACKWARD, deltaTime);
		if(Input::IsKeyPressed(GM_KEY_A))
			camera.ProcessKeyboardInput(LEFT,     deltaTime);
		if(Input::IsKeyPressed(GM_KEY_D))
			camera.ProcessKeyboardInput(RIGHT,    deltaTime);
		
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
		/*
		//Light(s)
		std::vector<PointLight> pointLights;
			//Pointlight 0
			Gogaman::PointLight pointLight0;
			pointLight0.SetPosition(glm::vec3(sin(glfwGetTime()) * 1.0f - 1.0f, 2.2f, -2.0f));
			//Luminous intensity (candela)
			pointLight0.SetColor(glm::vec3(3.0f, 8.0f, 11.0f));
			pointLights.push_back(pointLight0);
			//Pointlight 1
			Gogaman::PointLight pointLight1;
			pointLight1.SetPosition(glm::vec3(-2.0f, 1.2f, cos(glfwGetTime()) * 1.0f + 1.0f));
			//Luminous intensity (candela)
			pointLight1.SetColor(glm::vec3(11.0f, 8.0f, 4.0f));
			pointLights.push_back(pointLight1);
		*/
		
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
		glViewport(0, 0, m_RenderResolutionWidth, m_RenderResolutionHeight);
		glEnable(GL_DEPTH_TEST);
		m_G_Buffer->Bind();
		m_G_Buffer->Clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_ShaderManager->Get(m_GBufferShader).Bind();
		m_ShaderManager->Get(m_GBufferShader).SetUniformMat4("VP",                     viewProjectionMatrix);
		m_ShaderManager->Get(m_GBufferShader).SetUniformMat4("previousVP",             previousViewProjectionMatrix);
		m_ShaderManager->Get(m_GBufferShader).SetUniformVec2("temporalJitter",         glm::vec2(0.0f));
		m_ShaderManager->Get(m_GBufferShader).SetUniformVec2("previousTemporalJitter", glm::vec2(0.0f));
		m_ShaderManager->Get(m_GBufferShader).SetUniformBool("normalMapping",          GM_CONFIG.normalMapping);

		for(auto i : m_EntityGroups[GM_RENDERING_SYSTEM_RENDERABLE_GROUP_INDEX].entities)
		{
			RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i);

			m_ShaderManager->Get(m_GBufferShader).SetUniformMat4("M",         renderableComponent->modelMatrix);
			m_ShaderManager->Get(m_GBufferShader).SetUniformMat4("previousM", renderableComponent->modelMatrixHistory);
			
			renderableComponent->material.BindTextures();

			renderableComponent->vertexArrayBuffer->Bind();
			glDrawElements(GL_TRIANGLES, renderableComponent->indexBuffer->GetNumIndices(), GL_UNSIGNED_SHORT, 0);
		}
		
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

			m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3(positionUniformName, pointLightComponent->position);
			m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3(radianceUniformName, pointLightComponent->radiance);
			m_ShaderManager->Get(m_DeferredLightingShader).SetUniformFloat("pointLights[0].coneAperture", 0.0f);
			pointLightIndex++;
		}

		//m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3("pointLights[0].position",      pointLight0.GetPosition());
		//m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3("pointLights[0].color",         pointLight0.GetColor());
		//m_ShaderManager->Get(m_DeferredLightingShader).SetUniformFloat("pointLights[0].coneAperture", pointLight0.GetConeAperture());

		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("numLights",                     m_EntityGroups[GM_RENDERING_SYSTEM_POINT_LIGHT_GROUP_INDEX].entities.size());
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3("cameraPos",                    camera.Position);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformFloat("voxelGridSize",               GM_CONFIG.voxelGridSize);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformFloat("voxelGridSizeInverse",        1.0f / GM_CONFIG.voxelGridSize);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformFloat("voxelWorldSize",              GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformVec3("voxelGridPos",                 GM_CONFIG.voxelGridPos);

		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformInt("renderMode",                    GM_CONFIG.renderMode);
		m_ShaderManager->Get(m_DeferredLightingShader).SetUniformBool("debug2",                       GM_CONFIG.debug2);

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
		m_ShaderManager->Get(m_LightShader).SetUniformMat4("projection", projectionMatrix);
		m_ShaderManager->Get(m_LightShader).SetUniformMat4("view",       viewMatrix);
		//Light 0
		m_ShaderManager->Get(m_LightShader).SetUniformMat4("M",          glm::mat4());
		m_ShaderManager->Get(m_LightShader).SetUniformVec3("lightColor", pointLight0.GetColor());
		
		RenderFullscreenQuad();
		
			//Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Bind();
		glm::mat4 cubemapView(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.SetUniformMat4("projection", projectionMatrix);
		skyboxShader.SetUniformMat4("view", cubemapView);
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
		m_ShaderManager->Get(m_PostprocessShader).SetUniformFloat("exposureBias", exposure);
		m_ShaderManager->Get(m_PostprocessShader).SetUniformFloat("time",         (float)glfwGetTime());

		m_Texture2Ds["finalImage"].Bind(0);

		RenderFullscreenWindow();

		if(firstIteration == true)
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