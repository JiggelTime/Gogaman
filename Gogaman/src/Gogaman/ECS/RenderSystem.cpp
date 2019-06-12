#include "pch.h"
#include "RenderSystem.h"

#include "World.h"

namespace Gogaman
{
	RenderSystem::RenderSystem()
	{}

	void RenderSystem::Initialize()
	{/*
		//Initialize GLAD
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GM_ASSERT(gladStatus, "Failed to initialize GLAD");
		GM_LOG_CORE_INFO("OpenGL");
		GM_LOG_CORE_INFO("Version:  %s", glGetString(GL_VERSION));
		GM_LOG_CORE_INFO("Vendor:   %s", glGetString(GL_VENDOR));
		GM_LOG_CORE_INFO("Renderer: %s", glGetString(GL_RENDERER));
		
		//Initialize framebuffers
		InitializeFramebuffers();

		//Shaders
		ResourceManager::LoadShader("precomputeBRDFShader", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.vs", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.fs");
		ResourceManager::LoadShader("gbufferShader", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.vs", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.fs");
		ResourceManager::LoadShader("directPBRShader", "D:/dev/Gogaman/Gogaman/shaders/directPBR.vs", "D:/dev/Gogaman/Gogaman/shaders/directPBR.fs");
		ResourceManager::LoadShader("skyboxShader", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.vs", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.fs");
		ResourceManager::LoadShader("lampShader", "D:/dev/Gogaman/Gogaman/shaders/lampshader.vs", "D:/dev/Gogaman/Gogaman/shaders/lampshader.fs");
		ResourceManager::LoadShader("postProcessShader", "D:/dev/Gogaman/Gogaman/shaders/postprocess.vs", "D:/dev/Gogaman/Gogaman/shaders/postprocess.fs");

		//Configure global OpenGL state
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_TEXTURE_3D);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Configure fullscreen quad
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);

		//Generate BRDF LUT for split-sum approximation
		glViewport(0, 0, 512, 512);
		m_Framebuffers["BRDF_LUT_FB"].Bind();

		GM_SHADER(precomputeBRDFShader).Bind();

		RenderFullscreenQuad();

		//Set shader image units
		GM_SHADER(directPBRShader).Bind();
		GM_SHADER(directPBRShader).SetUniformInt("gPositionMetalness", 0);
		GM_SHADER(directPBRShader).SetUniformInt("gNormal", 1);
		GM_SHADER(directPBRShader).SetUniformInt("gAlbedoEmissivityRoughness", 2);
		GM_SHADER(directPBRShader).SetUniformInt("BRDF_LUT", 3);
		GM_SHADER(directPBRShader).SetUniformInt("coneTracedDiffuse", 4);
		GM_SHADER(directPBRShader).SetUniformInt("coneTracedSpecular", 5);
		GM_SHADER(directPBRShader).SetUniformInt("voxelTexture", 6);

		GM_SHADER(skyboxShader).Bind();
		GM_SHADER(skyboxShader).SetUniformInt("skybox", 0);

		GM_SHADER(postProcessShader).Bind();
		GM_SHADER(postProcessShader).SetUniformInt("hdrTexture", 0);*/
	}

	void RenderSystem::InitializeFramebuffers()
	{

	}

	void RenderSystem::Render()
	{
		std::cout << "[RenderSystem] Entities: " << m_Entities.size() << std::endl;

		for (auto i : m_Entities)
		{
			RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i);
			GM_LOG_CORE_INFO("[RenderSystem] Rendering %d vertices and %d indices", renderableComponent->data.vertexBufferData.size(), renderableComponent->data.indexBufferData.size());
		}
	}
}