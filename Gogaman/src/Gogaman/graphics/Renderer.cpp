#include "pch.h"
#include "Renderer.h"
#include "Gogaman/Logging/Log.h"
#include "Gogaman/ResourceManager.h"
#include "Framebuffers.h"
#include "JitterSequences.h"
#include "Lights/PointLight.h"

namespace Gogaman
{
	Renderer::Renderer()
	{
		//Initialize and configure GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//Create GLFW window
		m_Window = glfwCreateWindow(GM_CONFIG.screenWidth, GM_CONFIG.screenHeight, "Gogaman", NULL, NULL);
		if (m_Window == nullptr)
		{
			GM_LOG_CORE_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);
		//Vertical synchronization
		glfwSwapInterval(GM_CONFIG.vSync ? 1 : 0);

		//Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			GM_LOG_CORE_ERROR("Failed to initialize GLAD");
			return;
		}

		//Initialize AntTweakBar
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(GM_CONFIG.screenWidth, GM_CONFIG.screenHeight);
		m_TweakBar = TwNewBar("Debug");

		TwAddVarRO(m_TweakBar, "Cock Nigger", TW_TYPE_FLOAT, &GM_CONFIG.screenWidth, NULL);

		//Set GLFW event callbacks
		//glfwSetFramebufferSizeCallback(m_Window, WindowResizeCallback);
		//glfwSetCursorPosCallback(m_Window,       MouseMovedCallback);
		//glfwSetScrollCallback(m_Window,          MouseScrolledCallback);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//Initialize framebuffers
		Framebuffers::Initialize();
		GM_LOG_CORE_INFO("Optimization: change internal format of separable circular blur convolution textures from floating point");

		//Voxel textures
		voxelAlbedo.formatInternal = GL_RGBA8;
		voxelAlbedo.formatImage = GL_RGBA;
		voxelAlbedo.filterMin = GL_LINEAR_MIPMAP_LINEAR;
		voxelAlbedo.filterMag = GL_LINEAR;
		voxelAlbedo.levels = 0;
		voxelAlbedo.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelNormal.formatInternal = GL_RGBA8;
		voxelNormal.formatImage = GL_RGBA;
		voxelNormal.filterMin = GL_NEAREST;
		voxelNormal.filterMag = GL_NEAREST;
		voxelNormal.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelDirectRadiance.formatInternal = GL_RGBA8;
		voxelDirectRadiance.formatImage = GL_RGBA;
		voxelDirectRadiance.filterMin = GL_LINEAR_MIPMAP_LINEAR;
		voxelDirectRadiance.filterMag = GL_LINEAR;
		voxelDirectRadiance.levels = 0;
		voxelDirectRadiance.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelTotalRadiance.formatInternal = GL_RGBA8;
		voxelTotalRadiance.formatImage = GL_RGBA;
		voxelTotalRadiance.filterMin = GL_LINEAR_MIPMAP_LINEAR;
		voxelTotalRadiance.filterMag = GL_LINEAR;
		voxelTotalRadiance.levels = 0;
		voxelTotalRadiance.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelStaticFlag.formatInternal = GL_R8;
		voxelStaticFlag.formatImage = GL_RED;
		voxelStaticFlag.filterMin = GL_NEAREST;
		voxelStaticFlag.filterMag = GL_NEAREST;
		voxelStaticFlag.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		//Shaders
		Shader precomputeBRDFShader = ResourceManager::LoadShader("precomputeBRDFShader", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.vs", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.fs");
		Shader gbufferShader        = ResourceManager::LoadShader("gbufferShader", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.vs", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.fs");
		Shader downsampleNormalShader = ResourceManager::LoadShader("downsampleNormalShader", "D:/dev/Gogaman/Gogaman/shaders/downsampleNormal.vs", "D:/dev/Gogaman/Gogaman/shaders/downsampleNormal.fs");
		Shader downsampleDepthShader = ResourceManager::LoadShader("downsampleDepthShader", "D:/dev/Gogaman/Gogaman/shaders/downsampleDepth.vs", "D:/dev/Gogaman/Gogaman/shaders/downsampleDepth.fs");
		Shader voxelClearDynamicShader = ResourceManager::LoadShader("voxelClearDynamicShader", "D:/dev/Gogaman/Gogaman/shaders/voxelClearDynamic.compute");
		Shader voxelizationShader = ResourceManager::LoadShader("voxelizationShader", "D:/dev/Gogaman/Gogaman/shaders/voxelization.vs", "D:/dev/Gogaman/Gogaman/shaders/voxelization.fs", "D:/dev/Gogaman/Gogaman/shaders/voxelization.gs");
		Shader voxelInjectDirectShader = ResourceManager::LoadShader("voxelInjectDirectShader", "D:/dev/Gogaman/Gogaman/shaders/voxelInjectDirect.compute");
		Shader voxelInjectIndirectShader = ResourceManager::LoadShader("voxelInjectIndirectShader", "D:/dev/Gogaman/Gogaman/shaders/voxelInjectIndirect.compute");
		Shader voxelConeTracingShader = ResourceManager::LoadShader("voxelConeTracingShader", "D:/dev/Gogaman/Gogaman/shaders/voxelConeTracing.vs", "D:/dev/Gogaman/Gogaman/shaders/voxelConeTracing2.fs");
		Shader upsampleShader = ResourceManager::LoadShader("upsampleShader", "D:/dev/Gogaman/Gogaman/shaders/upsample.vs", "D:/dev/Gogaman/Gogaman/shaders/upsample.fs");
		Shader directPBRShader = ResourceManager::LoadShader("directPBRShader", "D:/dev/Gogaman/Gogaman/shaders/directPBR.vs", "D:/dev/Gogaman/Gogaman/shaders/directPBR.fs");
		Shader ssrShader = ResourceManager::LoadShader("ssrShader", "D:/dev/Gogaman/Gogaman/shaders/ssr.vs", "D:/dev/Gogaman/Gogaman/shaders/ssr.fs");
		//Shader combineIndirectShader = ResourceManager::LoadShader("combineIndirectShader", "D:/dev/Gogaman/Gogaman/shaders/combineIndirect.vs", "D:/dev/Gogaman/Gogaman/shaders/combineIndirect.fs");
		Shader skyboxShader = ResourceManager::LoadShader("skyboxShader", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.vs", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.fs");
		Shader lampShader = ResourceManager::LoadShader("lampShader", "D:/dev/Gogaman/Gogaman/shaders/lampshader.vs", "D:/dev/Gogaman/Gogaman/shaders/lampshader.fs");
		Shader taaShader = ResourceManager::LoadShader("taaShader", "D:/dev/Gogaman/Gogaman/shaders/taa.vs", "D:/dev/Gogaman/Gogaman/shaders/taa.fs");
		Shader circleOfConfusionShader = ResourceManager::LoadShader("circleOfConfusionShader", "D:/dev/Gogaman/Gogaman/shaders/coc.vs", "D:/dev/Gogaman/Gogaman/shaders/coc.fs");
		Shader circularBlurVerticalShader = ResourceManager::LoadShader("circularBlurVerticalShader", "D:/dev/Gogaman/Gogaman/shaders/circularBlurVertical.vs", "D:/dev/Gogaman/Gogaman/shaders/circularBlurVertical.fs");
		Shader circularBlurHorizontalShader = ResourceManager::LoadShader("circularBlurHorizontalShader", "D:/dev/Gogaman/Gogaman/shaders/circularBlurHorizontal.vs", "D:/dev/Gogaman/Gogaman/shaders/circularBlurHorizontal.fs");
		Shader gaussianBlurShader = ResourceManager::LoadShader("gaussianBlurShader", "D:/dev/Gogaman/Gogaman/shaders/gaussianblurshader.vs", "D:/dev/Gogaman/Gogaman/shaders/gaussianblurshader.fs");
		Shader bloomShader = ResourceManager::LoadShader("bloomShader", "D:/dev/Gogaman/Gogaman/shaders/bloom.vs", "D:/dev/Gogaman/Gogaman/shaders/bloom.fs");
		Shader postProcessShader = ResourceManager::LoadShader("postProcessShader", "D:/dev/Gogaman/Gogaman/shaders/postprocess.vs", "D:/dev/Gogaman/Gogaman/shaders/postprocess.fs");

		//Models
		Model roomModel   = ResourceManager::LoadModel("roomModel", "D:/ProgrammingStuff/Resources/Models/Test_Scene/Room.obj");
		Model redModel    = ResourceManager::LoadModel("redModel", "D:/ProgrammingStuff/Resources/Models/Test_Scene/Red.obj");
		Model blueModel   = ResourceManager::LoadModel("blueModel", "D:/ProgrammingStuff/Resources/Models/Test_Scene/Blue.obj");
		//Model statueModel = ResourceManager::LoadModel("statueModel", "D:/ProgrammingStuff/Resources/Models/Statue/Statue.obj");

		Model sphereModel = ResourceManager::LoadModel("sphereModel", "D:/ProgrammingStuff/Resources/Models/Sphere.obj");

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

		//Generate BRDF LUT
		glGenTextures(1, &brdfLUT);
		glBindTexture(GL_TEXTURE_2D, brdfLUT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::brdfFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, Framebuffers::brdfRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

		glViewport(0, 0, 512, 512);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		precomputeBRDFShader.Bind();

		RenderFullscreenQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Set shader image units
		downsampleNormalShader.Bind();
		downsampleNormalShader.SetUniformInt("normalTexture", 0);

		downsampleDepthShader.Bind();
		downsampleDepthShader.SetUniformInt("depthTexture", 0);

		voxelClearDynamicShader.Bind();
		voxelClearDynamicShader.SetUniformInt("voxelAlbedo", 0);
		voxelClearDynamicShader.SetUniformInt("voxelNormal", 1);
		voxelClearDynamicShader.SetUniformInt("voxelStaticFlag", 2);

		voxelizationShader.Bind();
		voxelizationShader.SetUniformInt("voxelAlbedo", 0);
		voxelizationShader.SetUniformInt("voxelNormal", 1);
		voxelizationShader.SetUniformInt("voxelStaticFlag", 2);

		voxelInjectDirectShader.Bind();
		voxelInjectDirectShader.SetUniformInt("voxelAlbedo", 0);
		voxelInjectDirectShader.SetUniformInt("voxelNormal", 1);
		voxelInjectDirectShader.SetUniformInt("voxelDirectRadiance", 2);

		voxelInjectIndirectShader.Bind();
		voxelInjectIndirectShader.SetUniformInt("voxelAlbedo", 0);
		voxelInjectIndirectShader.SetUniformInt("voxelNormal", 1);
		voxelInjectIndirectShader.SetUniformInt("voxelDirectRadiance", 2);
		voxelInjectIndirectShader.SetUniformInt("voxelTotalRadiance", 3);

		voxelConeTracingShader.Bind();
		voxelConeTracingShader.SetUniformInt("gPositionMetalness", 1);
		voxelConeTracingShader.SetUniformInt("gNormal", 2);
		voxelConeTracingShader.SetUniformInt("gAlbedoEmissivityRoughness", 3);

		upsampleShader.Bind();
		upsampleShader.SetUniformInt("inputTexture", 0);
		upsampleShader.SetUniformInt("previousInputTexture", 1);
		upsampleShader.SetUniformInt("depthTexture", 2);
		upsampleShader.SetUniformInt("depthCoarseTexture", 3);
		upsampleShader.SetUniformInt("normalTexture", 4);
		upsampleShader.SetUniformInt("normalCoarseTexture", 5);
		upsampleShader.SetUniformInt("velocityTexture", 6);

		directPBRShader.Bind();
		directPBRShader.SetUniformInt("gPositionMetalness", 0);
		directPBRShader.SetUniformInt("gNormal", 1);
		directPBRShader.SetUniformInt("gAlbedoEmissivityRoughness", 2);
		directPBRShader.SetUniformInt("brdfLUT", 3);
		directPBRShader.SetUniformInt("coneTracedDiffuse", 4);
		directPBRShader.SetUniformInt("coneTracedSpecular", 5);

		ssrShader.Bind();
		ssrShader.SetUniformInt("renderedImageTexture", 0);
		ssrShader.SetUniformInt("depthTexture", 1);
		ssrShader.SetUniformInt("normalTexture", 2);

		//combineIndirectShader.Bind();
		//combineIndirectShader.SetUniformInt("")
		//combineIndirectShader.SetUniformInt("")

		skyboxShader.Bind();
		skyboxShader.SetUniformInt("skybox", 0);

		taaShader.Bind();
		taaShader.SetUniformInt("inputTexture", 0);
		taaShader.SetUniformInt("previousInputTexture", 1);
		taaShader.SetUniformInt("depthTexture", 2);
		taaShader.SetUniformInt("velocityTexture", 3);

		circleOfConfusionShader.Bind();
		circleOfConfusionShader.SetUniformInt("imageTexture", 0);
		circleOfConfusionShader.SetUniformInt("depthTexture", 1);

		circularBlurHorizontalShader.Bind();
		circularBlurHorizontalShader.SetUniformInt("cocTexture", 0);
		circularBlurHorizontalShader.SetUniformInt("imageTexture", 1);

		circularBlurVerticalShader.Bind();
		circularBlurVerticalShader.SetUniformInt("cocTexture", 0);
		circularBlurVerticalShader.SetUniformInt("imageRedChannelTexture", 1);
		circularBlurVerticalShader.SetUniformInt("imageGreenChannelTexture", 2);
		circularBlurVerticalShader.SetUniformInt("imageBlueChannelTexture", 3);

		gaussianBlurShader.Bind();
		gaussianBlurShader.SetUniformInt("imageTexture", 0);

		bloomShader.Bind();
		bloomShader.SetUniformInt("imageTexture", 0);
		bloomShader.SetUniformInt("bloomTexture", 1);

		postProcessShader.Bind();
		postProcessShader.SetUniformInt("hdrTexture", 0);

		//Generate query
		glGenQueries(1, &query);
	}

	Renderer::~Renderer()
	{}

	void Renderer::Render()
	{
		//Timing
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameCounter++;

		//Input
		ProcessInput(m_Window);

		//Update sub-pixel jitter for temporal resolve
		previousTemporalJitter = temporalJitter;
		temporalJitter = (halton16[temporalOffsetIterator] - 0.5f) * screenTexelSize;
		temporalJitter *= GM_CONFIG.temporalJitterSpread;
		if(!GM_CONFIG.taa)
			temporalJitter = glm::vec2(0.0f);

		coneTraceJitter = (halton16[temporalOffsetIterator] - 0.5f) * screenTexelSize;
		coneTraceJitter *= 1.0f / GM_CONFIG.giResScale;
		//if(!GM_CONFIG.giUpscaling)
		coneTraceJitter = glm::vec2(0.0f);

		temporalOffsetIterator = temporalOffsetIterator >= 15 ? 0 : temporalOffsetIterator + 1;

		//Create translation matrix that translates by the jitter in x and y axis
		glm::mat4 jitterTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(temporalJitter, 0.0f));

		//Update light(s)
		//Pointlight 0
		Gogaman::PointLight pointLight0;
		pointLight0.SetPosition(glm::vec3(0.4f, 1.2f, -0.6f));
		//Luminous intensity (candela)
		pointLight0.SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		//Pointlight 1
		Gogaman::PointLight pointLight1;
		pointLight1.SetPosition(glm::vec3(-0.4f, 1.2f, 0.6f));
		//Luminous intensity (candela)
		pointLight1.SetColor(glm::vec3(2.0f, 2.0f, 2.0f));
		
		//Update models
		GM_MODEL(statueModel).SetScale(0.4f);
		GM_MODEL(statueModel).SetPosition(glm::vec3(sin(glfwGetTime() * 2.0f) * 1.2f, 0.0f, 0.0f));

	//Update camera matrices
		previousViewProjectionMatrix = viewProjectionMatrix;
		projectionMatrix = glm::perspective(glm::radians(camera.Zoom), aspectRatio, cameraNearPlane, cameraFarPlane);
		//Apply jitter
		projectionMatrix = jitterTranslationMatrix * projectionMatrix;
		viewMatrix = camera.GetViewMatrix();
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		//Voxel camera
			//voxelGridPos = camera.Position;
		GM_CONFIG.voxelGridSize *= 2.0f;
		auto voxelHalfGridSize = GM_CONFIG.voxelGridSize / 2.0f;
		glm::mat4 voxelProjection = glm::ortho(-voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize, 0.0f, GM_CONFIG.voxelGridSize);
		//if(debug) voxelProjection = glm::ortho(-voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize);
		glm::mat4 voxelView = glm::lookAt(GM_CONFIG.voxelGridPos, GM_CONFIG.voxelGridPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 voxelViewProjection = voxelProjection * voxelView;
		GM_CONFIG.voxelGridSize /= 2.0f;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//Set render mode to wireframe if enabled
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Geometry pass
			//Begin geometry pass timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GM_SHADER(gbufferShader).Bind();
		GM_SHADER(gbufferShader).SetUniformMat4("VP", viewProjectionMatrix);
		GM_SHADER(gbufferShader).SetUniformMat4("previousVP", previousViewProjectionMatrix);
		GM_SHADER(gbufferShader).SetUniformVec2("temporalJitter", temporalJitter);
		GM_SHADER(gbufferShader).SetUniformVec2("previousTemporalJitter", previousTemporalJitter);
		GM_SHADER(gbufferShader).SetUniformBool("normalMapping", GM_CONFIG.normalMapping);
		GM_SHADER(gbufferShader).SetUniformBool("emissive", false);

		GM_SHADER(gbufferShader).SetUniformBool("debug", GM_CONFIG.debug);

		for(auto i : ResourceManager::models)
			i.second.Render(GM_SHADER(gbufferShader), true);

		//Remove me if the above iterator works :)
		//GM_MODEL(roomModel).Render(GM_SHADER(gbufferShader), true);
		//GM_MODEL(redModel).Render(GM_SHADER(gbufferShader), true);
		//GM_MODEL(blueModel).Render(GM_SHADER(gbufferShader), true);
		//GM_MODEL(statueModel).Render(GM_SHADER(gbufferShader), true);

		glDisable(GL_DEPTH_TEST);

		//End geometry pass timer
		glEndQuery(GL_TIME_ELAPSED);
		//Wait until the query result are available
		timerResultAvailable = 0;
		while (!timerResultAvailable)
			glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

		//Get query result
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
		previousTimeGeometryPass = timeGeometryPass;
		timeGeometryPass = elapsedTime / 1000000.0f;

		//Downsample normal
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::normalDownsampleFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		GM_SHADER(downsampleNormalShader).Bind();
		GM_SHADER(downsampleNormalShader).SetUniformBool("debug", GM_CONFIG.debug);

		int maxNormalMipLevels = floor(log2(std::max(GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale)));
		maxNormalMipLevels = 6;
		for(int i = 0; i < maxNormalMipLevels; i++)
		{
			int sampleMipLevel = i == 0 ? 0 : i - 1;
			int mipWidth  = floor(GM_CONFIG.screenWidth  * GM_CONFIG.resScale * 0.5f * std::pow(0.5f, i));
			int mipHeight = floor(GM_CONFIG.screenHeight * GM_CONFIG.resScale * 0.5f * std::pow(0.5f, i));
			glViewport(0, 0, mipWidth, mipHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffers::normalDownsampleBuffer.GetID(), i);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(downsampleNormalShader).SetUniformInt("sampleMipLevel", sampleMipLevel);
			if(i == 0)
				Framebuffers::gNormal.BindTextureUnit(0);
			else
				Framebuffers::normalDownsampleBuffer.BindTextureUnit(0);

			RenderFullscreenQuad();
		}

		//Downsample depth
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::depthDownsampleFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		GM_SHADER(downsampleDepthShader).Bind();
		GM_SHADER(downsampleDepthShader).SetUniformBool("debug", GM_CONFIG.debug);

		int maxDepthMipLevels = floor(log2(std::max(GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale)));
		maxDepthMipLevels = 6;
		for(int i = 0; i < maxDepthMipLevels; i++)
		{
			int sampleMipLevel = i == 0 ? 0 : i - 1;
			int mipWidth  = floor(GM_CONFIG.screenWidth  * GM_CONFIG.resScale * 0.5f * std::pow(0.5f, i));
			int mipHeight = floor(GM_CONFIG.screenHeight * GM_CONFIG.resScale * 0.5f * std::pow(0.5f, i));
			glViewport(0, 0, mipWidth, mipHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffers::depthDownsampleBuffer.GetID(), i);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(downsampleDepthShader).SetUniformInt("sampleMipLevel", sampleMipLevel);
			if(i == 0)
				Framebuffers::gDepth.BindTextureUnit(0);
			else
				Framebuffers::depthDownsampleBuffer.BindTextureUnit(0);

			RenderFullscreenQuad();
		}

		//Reset render mode back to fill
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Begin VCTGI timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		//Voxelization
		if((GM_CONFIG.autoVoxelize == true && voxelizationCounter >= GM_CONFIG.voxelizationFrequency) || firstIteration == true)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
			glEnable(GL_CONSERVATIVE_RASTERIZATION_INTEL);

			glViewport(0, 0, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Voxelize static models
			if (firstIteration == true)
			{
				GM_SHADER(voxelizationShader).Bind();
				GM_SHADER(voxelizationShader).SetUniformMat4("VP", voxelViewProjection);
				GM_SHADER(voxelizationShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
				GM_SHADER(voxelizationShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
				GM_SHADER(voxelizationShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);
				GM_SHADER(voxelizationShader).SetUniformBool("flagStatic", true);
				GM_SHADER(voxelizationShader).SetUniformBool("emissive", false);

				GM_SHADER(voxelizationShader).SetUniformBool("debug", GM_CONFIG.debug);

				GM_SHADER(voxelizationShader).SetUniformInt("voxelAlbedo", 0);
				voxelAlbedo.BindTextureUnit(0);
				GM_SHADER(voxelizationShader).SetUniformInt("voxelNormal", 1);
				voxelNormal.BindTextureUnit(1);
				GM_SHADER(voxelizationShader).SetUniformInt("voxelStaticFlag", 2);
				voxelStaticFlag.BindTextureUnit(2);

				//Clear voxel textures
				voxelAlbedo.Clear();
				voxelNormal.Clear();
				voxelStaticFlag.Clear();

				//Bind voxel textures
				voxelAlbedo.BindImageUnit(0, 0, GL_READ_WRITE, GL_R32UI);
				voxelNormal.BindImageUnit(1, 0, GL_READ_WRITE, GL_R32UI);
				voxelStaticFlag.BindImageUnit(2, 0, GL_READ_ONLY, GL_R8);

				//Draw static models
				//GM_MODEL(roomModel).Render(GM_SHADER(voxelizationShader));
				//GM_MODEL(redModel).Render(GM_SHADER(voxelizationShader));
				//GM_MODEL(blueModel).Render(GM_SHADER(voxelizationShader));
				for(auto i : ResourceManager::models)
				{
					if(!i.second.IsDynamic())
						i.second.Render(GM_SHADER(voxelizationShader));
				}
				
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
			}

			//Clear dynamic voxels
			GM_SHADER(voxelClearDynamicShader).Bind();
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);

			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelAlbedo", 0);
			voxelAlbedo.BindTextureUnit(0);
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelNormal", 1);
			voxelNormal.BindTextureUnit(1);
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelStaticFlag", 2);
			voxelStaticFlag.BindTextureUnit(2);

			voxelAlbedo.BindImageUnit(0, 0, GL_READ_WRITE, GL_RGBA8);
			voxelNormal.BindImageUnit(1, 0, GL_WRITE_ONLY, GL_RGBA8);
			
			glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

			GM_SHADER(voxelizationShader).Bind();
			GM_SHADER(voxelizationShader).SetUniformMat4("VP", voxelViewProjection);
			GM_SHADER(voxelizationShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
			GM_SHADER(voxelizationShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
			GM_SHADER(voxelizationShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);
			GM_SHADER(voxelizationShader).SetUniformBool("flagStatic", false);

			voxelAlbedo.BindImageUnit(0, 0, GL_READ_WRITE, GL_R32UI);
			voxelNormal.BindImageUnit(1, 0, GL_READ_WRITE, GL_R32UI);
			voxelStaticFlag.BindImageUnit(2, 0, GL_READ_ONLY, GL_R8);

			//Render dynamic models
			//statueModel.Render(voxelizationShader);
			for(auto i : ResourceManager::models)
			{
				if(i.second.IsDynamic())
					i.second.Render(GM_SHADER(voxelizationShader));
			}

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_CULL_FACE);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_INTEL);

			voxelizationCounter = 0;
		}

		//Voxel direct light injection
		GM_SHADER(voxelInjectDirectShader).Bind();
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[0].position", pointLight0.GetPosition());
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[0].color", pointLight0.GetColor());
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("pointLights[0].coneAperture", pointLight0.GetConeAperture());
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("pointLights[1].coneAperture", pointLight1.GetConeAperture());
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("numLights", 1);
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);

		GM_SHADER(voxelInjectDirectShader).SetUniformBool("debug", GM_CONFIG.debug);

		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelAlbedo", 0);
		voxelAlbedo.BindTextureUnit(0);
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelNormal", 1);
		voxelNormal.BindTextureUnit(1);

		voxelDirectRadiance.BindImageUnit(2, 0, GL_WRITE_ONLY, GL_RGBA8);
		voxelDirectRadiance.Clear();

		glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		voxelDirectRadiance.GenerateMipmap();

		//Voxel indirect light injection
		GM_SHADER(voxelInjectIndirectShader).Bind();
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
		GM_SHADER(voxelInjectIndirectShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectIndirectShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectIndirectShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);

		GM_SHADER(voxelInjectIndirectShader).SetUniformBool("debug", GM_CONFIG.debug2);

		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelAlbedo", 0);
		voxelAlbedo.BindTextureUnit(0);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelNormal", 1);
		voxelNormal.BindTextureUnit(1);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelDirectRadiance", 2);
		voxelDirectRadiance.BindTextureUnit(2);

		voxelTotalRadiance.BindImageUnit(3, 0, GL_WRITE_ONLY, GL_RGBA8);
		voxelTotalRadiance.Clear();

		glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		voxelTotalRadiance.GenerateMipmap();

		//Screen space voxel Cone Tracing
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.giResScale, GM_CONFIG.screenHeight * GM_CONFIG.giResScale);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::indirectFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		GM_SHADER(voxelConeTracingShader).Bind();
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[0].position", pointLight0.GetPosition());
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[0].color", pointLight0.GetColor());
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		GM_SHADER(voxelConeTracingShader).SetUniformInt("numLights", 1);
		GM_SHADER(voxelConeTracingShader).SetUniformInt("renderMode", GM_CONFIG.renderMode);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelWorldSize", GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("cameraPos", camera.Position);
		GM_SHADER(voxelConeTracingShader).SetUniformVec2("coordJitter", coneTraceJitter);

		GM_SHADER(voxelConeTracingShader).SetUniformBool("debug", GM_CONFIG.debug);

		Framebuffers::gPositionMetalness.BindTextureUnit(1);
		Framebuffers::gNormal.BindTextureUnit(2);
		Framebuffers::gAlbedoEmissivityRoughness.BindTextureUnit(3);

		GM_SHADER(voxelConeTracingShader).SetUniformInt("voxelTotalRadiance", 4);
		voxelTotalRadiance.BindTextureUnit(4);
		
		RenderFullscreenQuad();

		//End VCTGI timer
		glEndQuery(GL_TIME_ELAPSED);
		//Wait until the query result are available
		timerResultAvailable = 0;
		while (!timerResultAvailable)
			glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

		//Get query result
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
		previousTimeVCTGI = timeVCTGI;
		timeVCTGI = elapsedTime / 1000000.0f;

		//Upsample indirect lighting
		if(GM_CONFIG.giUpscaling)
		{
			//Diffuse
			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::upsampleFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(upsampleShader).Bind();
			GM_SHADER(upsampleShader).SetUniformFloat("nearPlane", cameraNearPlane);
			GM_SHADER(upsampleShader).SetUniformFloat("farPlane", cameraFarPlane);
			GM_SHADER(upsampleShader).SetUniformInt("sampleTextureLod", std::max(floor(log2(1.0f / GM_CONFIG.giResScale)) - 1.0f, 0.0f));

			GM_SHADER(upsampleShader).SetUniformBool("debug", GM_CONFIG.debug);
			GM_SHADER(upsampleShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			Framebuffers::indirectLightingBuffers[0].BindTextureUnit(0);
			Framebuffers::previousUpsampleBuffer.BindTextureUnit(1);
			Framebuffers::gDepth.BindTextureUnit(2);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				Framebuffers::depthDownsampleBuffer.BindTextureUnit(3);
			else
				Framebuffers::gDepth.BindTextureUnit(3);
			Framebuffers::gNormal.BindTextureUnit(4);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				Framebuffers::normalDownsampleBuffer.BindTextureUnit(5);
			else
				Framebuffers::gNormal.BindTextureUnit(5);
			Framebuffers::gVelocity.BindTextureUnit(6);

			RenderFullscreenQuad();

			//Copy upscaled image to history buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers::previousUpsampleFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffers::upsampleFBO);
			glBlitFramebuffer(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, 0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			//Specular
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::upsampleFBO2);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(upsampleShader).Bind();
			GM_SHADER(upsampleShader).SetUniformFloat("nearPlane", cameraNearPlane);
			GM_SHADER(upsampleShader).SetUniformFloat("farPlane", cameraFarPlane);
			GM_SHADER(upsampleShader).SetUniformInt("sampleTextureLod", std::max(floor(log2(1.0f / GM_CONFIG.giResScale)) - 1.0f, 0.0f));

			GM_SHADER(upsampleShader).SetUniformBool("debug", GM_CONFIG.debug);
			GM_SHADER(upsampleShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			Framebuffers::indirectLightingBuffers[1].BindTextureUnit(0);
			Framebuffers::previousUpsampleBuffer2.BindTextureUnit(1);
			Framebuffers::gDepth.BindTextureUnit(2);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				Framebuffers::depthDownsampleBuffer.BindTextureUnit(3);
			else
				Framebuffers::gDepth.BindTextureUnit(3);
			Framebuffers::gNormal.BindTextureUnit(4);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				Framebuffers::normalDownsampleBuffer.BindTextureUnit(5);
			else
				Framebuffers::gNormal.BindTextureUnit(5);
			Framebuffers::gVelocity.BindTextureUnit(6);

			RenderFullscreenQuad();

			//Copy upscaled image to history buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers::previousUpsampleFBO2);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffers::upsampleFBO2);
			glBlitFramebuffer(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, 0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		//Begin deferred shading timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		//TODO: Render shadows at half-resolution (possibly quarter) stored in RGBA8 texture (possibly with 2 maps packed into each 8bit channel)
		//Do SSR with reprojected frame and VCT fallback. Upscale shadows, diffuse, and specular all in one pass, using MRT's to store results.
		//Possibly store upsampled indirect radiance in one texture (chroma subsampling).

	//Deferred shading
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GM_SHADER(directPBRShader).Bind();
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[0].position", pointLight0.GetPosition());
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[0].color", pointLight0.GetColor());
		GM_SHADER(directPBRShader).SetUniformFloat("pointLights[0].coneAperture", pointLight0.GetConeAperture());
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		GM_SHADER(directPBRShader).SetUniformFloat("pointLights[1].coneAperture", pointLight1.GetConeAperture());
		GM_SHADER(directPBRShader).SetUniformInt("numLights", 1);
		GM_SHADER(directPBRShader).SetUniformVec3("cameraPos", camera.Position);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelWorldSize", GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		GM_SHADER(directPBRShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);

		GM_SHADER(directPBRShader).SetUniformInt("renderMode", GM_CONFIG.renderMode);
		GM_SHADER(directPBRShader).SetUniformBool("debug", GM_CONFIG.debug);
		GM_SHADER(directPBRShader).SetUniformBool("debug2", GM_CONFIG.debug2);

		Framebuffers::gPositionMetalness.BindTextureUnit(0);
		Framebuffers::gNormal.BindTextureUnit(1);
		Framebuffers::gAlbedoEmissivityRoughness.BindTextureUnit(2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, brdfLUT);
		if(GM_CONFIG.giUpscaling)
			Framebuffers::upsampleBuffer.BindTextureUnit(4);
		else
			Framebuffers::indirectLightingBuffers[0].BindTextureUnit(4);
		glActiveTexture(GL_TEXTURE5);
		if(GM_CONFIG.giUpscaling)
			Framebuffers::upsampleBuffer2.BindTextureUnit(5);
		else
			Framebuffers::indirectLightingBuffers[1].BindTextureUnit(5);

		GM_SHADER(directPBRShader).SetUniformInt("voxelTexture", 6);
		voxelTotalRadiance.BindTextureUnit(6);

		RenderFullscreenQuad();

		//End deferred shading timer
		glEndQuery(GL_TIME_ELAPSED);
		//Wait until the query result are available
		timerResultAvailable = 0;
		while(!timerResultAvailable)
			glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

		//Get query result
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
		previousTimeDirectPBR = timeDirectPBR;
		timeDirectPBR = elapsedTime / 1000000.0f;

		//Copy gBuffer depth to HDR FBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffers::gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers::hdrFBO);
		glBlitFramebuffer(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, 0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::hdrFBO);

		//Forward rendering
		glEnable(GL_DEPTH_TEST);

		//Lights
		GM_SHADER(lampShader).Bind();
		GM_SHADER(lampShader).SetUniformMat4("projection", projectionMatrix);
		GM_SHADER(lampShader).SetUniformMat4("view", viewMatrix);
		//Light 0
		GM_SHADER(lampShader).SetUniformVec3("lightColor", pointLight0.GetColor());
		GM_MODEL(sphereModel).SetPosition(pointLight0.GetPosition());
		GM_MODEL(sphereModel).SetScale(0.025f);
		GM_MODEL(sphereModel).Render(GM_SHADER(lampShader));
		//Light 1
			//lampShader.SetUniformVec3("lightColor", pointLight1.color);
			//sphereModel.SetPosition(pointLight1.position);
			//sphereModel.SetScale(0.025f);
			//sphereModel.Render(lampShader);
		/*
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
		
	//Screen space indirect specular
		ssrShader.Bind();
		ssrShader.SetUniformMat3("transposeInverseV", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));
		ssrShader.SetUniformMat4("P",                 projectionMatrix);
		ssrShader.SetUniformMat4("inverseP",          glm::inverse(projectionMatrix));

		ssrShader.SetUniformBool("debug",             GM_CONFIG.debug);
		ssrShader.SetUniformBool("debug2",            GM_CONFIG.debug2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Framebuffers::colorBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Framebuffers::gDepth);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Framebuffers::gNormal);
		
		RenderFullscreenQuad();

	//Combine indirect lighting
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::ssrFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		combineIndirectShader.Bind();

		combineIndirectShader.SetUniformBool("debug", GM_CONFIG.debug);
		combineIndirectShader.SetUniformBool("debug2", GM_CONFIG.debug2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Framebuffers::colorBuffers[0]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Framebuffers::gAlbedoEmissivityRoughness);
		
		RenderFullscreenQuad();
		*/

		//Begin temporal anti-aliasing timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		//Temporal anti-aliasing
		if(GM_CONFIG.taa)
		{
			GM_SHADER(taaShader).Bind();

			GM_SHADER(taaShader).SetUniformBool("debug", GM_CONFIG.debug);
			GM_SHADER(taaShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			Framebuffers::colorBuffers[0].BindTextureUnit(0);
			Framebuffers::previousFrameBuffer.BindTextureUnit(1);
			Framebuffers::gDepth.BindTextureUnit(2);
			Framebuffers::gVelocity.BindTextureUnit(3);
			
			RenderFullscreenQuad();

			//Copy frame to history buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffers::previousFrameFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Framebuffers::hdrFBO);
			glBlitFramebuffer(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, 0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		//End temporal anti-aliasing timer
		glEndQuery(GL_TIME_ELAPSED);
		//Wait until the query result are available
		timerResultAvailable = 0;
		while (!timerResultAvailable)
			glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

		//Get query result
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
		previousTimeTAA = timeTAA;
		timeTAA = elapsedTime / 1000000.0f;

		//Depth of field
		if(GM_CONFIG.dof)
		{
			//Begin depth of field timer
			glBeginQuery(GL_TIME_ELAPSED, query);

			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.dofResScale, GM_CONFIG.screenHeight * GM_CONFIG.dofResScale);

			//Compute circle of confusion
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::cocFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circleOfConfusionShader).Bind();
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("nearPlane", cameraNearPlane);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("farPlane", cameraFarPlane);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("focalDistance", GM_CONFIG.focalDistance);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("fStop", GM_CONFIG.fStop);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("focalLength", GM_CONFIG.focalLength);

			GM_SHADER(circleOfConfusionShader).SetUniformBool("debug", GM_CONFIG.debug);
			GM_SHADER(circleOfConfusionShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			Framebuffers::gDepth.BindTextureUnit(0);
			
			RenderFullscreenQuad();

			//Circular blur
				//Horizontal pass
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::circularBlurHorizontalFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circularBlurHorizontalShader).Bind();

			GM_SHADER(circularBlurHorizontalShader).SetUniformBool("debug", GM_CONFIG.debug);

			Framebuffers::cocBuffer.BindTextureUnit(0);
			Framebuffers::colorBuffers[0].BindTextureUnit(1);
			
			RenderFullscreenQuad();

			//Vertical pass and final composite
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::circularBlurVerticalFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circularBlurVerticalShader).Bind();

			GM_SHADER(circularBlurVerticalShader).SetUniformBool("debug", GM_CONFIG.debug);

			Framebuffers::cocBuffer.BindTextureUnit(0);
			Framebuffers::circularBlurRedBuffer.BindTextureUnit(1);
			Framebuffers::circularBlurGreenBuffer.BindTextureUnit(2);
			Framebuffers::circularBlurBlueBuffer.BindTextureUnit(3);
			
			RenderFullscreenQuad();

			//End depth of field timer
			glEndQuery(GL_TIME_ELAPSED);
			//Wait until the query result are available
			timerResultAvailable = 0;
			while(!timerResultAvailable)
				glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

			//Get query result
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
			previousTimeDOF = timeDOF;
			timeDOF = elapsedTime / 1000000.0f;
		}

		//Bloom
		if(GM_CONFIG.bloom)
		{
			//Begin bloom timer
			glBeginQuery(GL_TIME_ELAPSED, query);

			//Separable Gaussian blur
			bool horizontal = true, firstBlurIteration = true;
			//Blur radius is resolution independent
			float screenDiagonal = glm::sqrt((GM_CONFIG.screenWidth * GM_CONFIG.screenWidth) + (GM_CONFIG.screenHeight * GM_CONFIG.screenHeight));
			uint bloomBlurIterations = glm::ceil(screenDiagonal * GM_CONFIG.bloomBlurAmount);

			GM_SHADER(gaussianBlurShader).Bind();

			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.bloomResScale, GM_CONFIG.screenHeight * GM_CONFIG.bloomResScale);
			for(uint i = 0; i < bloomBlurIterations; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::pingpongFBO[horizontal]);
				if(firstBlurIteration)
					glClear(GL_COLOR_BUFFER_BIT);
				GM_SHADER(gaussianBlurShader).SetUniformBool("horizontal", horizontal);
				if(firstBlurIteration)
					Framebuffers::colorBuffers[1].BindTextureUnit(0);
				else
					Framebuffers::pingpongColorbuffers[!horizontal].BindTextureUnit(0);
				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
				horizontal = !horizontal;
				if(firstBlurIteration)
					firstBlurIteration = false;
			}

			//Apply bloom to image
			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffers::hdrFBO);

			GM_SHADER(bloomShader).Bind();
			GM_SHADER(bloomShader).SetUniformFloat("bloomStrength", GM_CONFIG.bloomStrength);

			Framebuffers::colorBuffers[0].BindTextureUnit(0);
			Framebuffers::pingpongColorbuffers[!horizontal].BindTextureUnit(1);
			
			RenderFullscreenQuad();

			//End bloom timer
			glEndQuery(GL_TIME_ELAPSED);
			//Wait until the query result are available
			timerResultAvailable = 0;
			while(!timerResultAvailable)
				glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

			//Get query result
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
			previousTimeBloom = timeBloom;
			timeBloom = elapsedTime / 1000000.0f;
		}

		//Post-process (tonemapping, exposure, film grain, gamma correction)
		glViewport(0, 0, GM_CONFIG.screenWidth, GM_CONFIG.screenHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GM_SHADER(postProcessShader).Bind();
		GM_SHADER(postProcessShader).SetUniformFloat("exposureBias", exposure);
		GM_SHADER(postProcessShader).SetUniformFloat("time", glfwGetTime());

		GM_SHADER(postProcessShader).SetUniformBool("debug", GM_CONFIG.debug);

		if(GM_CONFIG.dof)
			Framebuffers::circularBlurVerticalBuffer.BindTextureUnit(0);
		else
			Framebuffers::colorBuffers[0].BindTextureUnit(0);
		
		RenderFullscreenQuad();

		//Print settings and timings
		//std::cout << "Exposure: " << exposure << " | DOF: " << (GM_CONFIG.dof ? "ON" : "OFF") << " | Bloom: " << (GM_CONFIG.bloom ? "ON" : "OFF") << " | Normal mapping: " << (GM_CONFIG.normalMapping ? "ON" : "OFF") << " | Debug: " << (GM_CONFIG.debug ? "ON" : "OFF") << " | Debug 2: " << (GM_CONFIG.debug2 ? "ON" : "OFF") << " | TAA: " << (GM_CONFIG.taa ? "ON" : "OFF") << " | AutoVoxelize: " << GM_CONFIG.autoVoxelize << std::endl;
		//std::cout << "" << std::endl;
		//std::cout << std::fixed << std::setprecision(2) << "Timings (ms); geometry pass: " << (timeGeometryPass + previousTimeGeometryPass) / 2.0f << ", VCTGI: " << (timeVCTGI + previousTimeVCTGI) / 2.0f << ", direct PBR: " << (timeDirectPBR + previousTimeDirectPBR) / 2.0f << ", TAA: " << (timeTAA + previousTimeTAA) / 2.0f << ", DOF: " << (timeDOF + previousTimeDOF) / 2.0f << ", bloom: " << (timeBloom + previousTimeBloom) / 2.0f << std::endl;
		//std::cout << "" << std::endl;

		voxelizationCounter++;

		if(firstIteration == true)
			firstIteration = false;

		//Render debug GUI
		TwDraw();

		//GLFW: Swap buffers and poll IO events
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void Renderer::RenderFullscreenQuad() const
	{
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void Renderer::ProcessInput(GLFWwindow *window)
	{
		//Close window
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		//Movement
		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboardInput(FORWARD, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboardInput(BACKWARD, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboardInput(LEFT, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboardInput(RIGHT, deltaTime);
		
		//Toggle depth of field
		if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !GM_CONFIG.dof)
		{
			GM_CONFIG.dof = !GM_CONFIG.dof;
			GM_CONFIG.dofKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
		{
			GM_CONFIG.dofKeyPressed = false;
		}
		//Enable/disable bloom
		if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !GM_CONFIG.bloomKeyPressed)
		{
			GM_CONFIG.bloom = !GM_CONFIG.bloom;
			GM_CONFIG.bloomKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
		{
			GM_CONFIG.bloomKeyPressed = false;
		}
		//Enable/disable debug mode
		if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !GM_CONFIG.debugKeyPressed)
		{
			GM_CONFIG.debug = !GM_CONFIG.debug;
			GM_CONFIG.debugKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		{
			GM_CONFIG.debugKeyPressed = false;
		}
		//Enable/disable debug mode 2
		if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !GM_CONFIG.debug2KeyPressed)
		{
			GM_CONFIG.debug2 = !GM_CONFIG.debug2;
			GM_CONFIG.debug2KeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
		{
			GM_CONFIG.debug2KeyPressed = false;
		}
		//Toggle spatio-temporal indirect lighting upscaling
		if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !GM_CONFIG.giUpscalingKeyPressed)
		{
			GM_CONFIG.giUpscaling = !GM_CONFIG.giUpscaling;
			GM_CONFIG.giUpscalingKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
		{
			GM_CONFIG.giUpscalingKeyPressed = false;
		}
		//Toggle automatic revoxelization
		if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !GM_CONFIG.autoVoxelizeKeyPressed)
		{
			GM_CONFIG.autoVoxelize = !GM_CONFIG.autoVoxelize;
			GM_CONFIG.autoVoxelizeKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		{
			GM_CONFIG.autoVoxelizeKeyPressed = false;
		}
		//Enable/disable normal mapping
		if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !GM_CONFIG.normalMappingKeyPressed)
		{
			GM_CONFIG.normalMapping = !GM_CONFIG.normalMapping;
			GM_CONFIG.normalMappingKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
		{
			GM_CONFIG.normalMappingKeyPressed = false;
		}
		//Adjust exposure
		if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			if(exposure > 0.011f)
				exposure -= 0.012f;
			else
				exposure = 0.0f;
		}
		else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			exposure += 0.012f;
		}
		//Enable/disable wireframe rendering
		if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !GM_CONFIG.wireframeKeyPressed)
		{
			GM_CONFIG.wireframe = !GM_CONFIG.wireframe;
			GM_CONFIG.wireframeKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
			GM_CONFIG.wireframeKeyPressed = false;
		//Enable/disable temporal anti-aliasing
		if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !GM_CONFIG.taaKeyPressed)
		{
			GM_CONFIG.taa = !GM_CONFIG.taa;
			GM_CONFIG.taaKeyPressed = true;
		}
		if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		{
			GM_CONFIG.taaKeyPressed = false;
		}
		//Set render mode
		if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
			GM_CONFIG.renderMode = 0;
		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			GM_CONFIG.renderMode = 1;
		if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			GM_CONFIG.renderMode = 2;
		if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			GM_CONFIG.renderMode = 3;
		if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			GM_CONFIG.renderMode = 4;
		if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			GM_CONFIG.renderMode = 5;
		if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
			GM_CONFIG.renderMode = 6;
		if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
			GM_CONFIG.renderMode = 7;
		if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
			GM_CONFIG.renderMode = 8;
	}

	void Renderer::WindowResizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::MouseMovedCallback(GLFWwindow *window, double xPos, double yPos)
	{
		if(firstMouse)
		{
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float xOffset = xPos - lastX;
		//Reversed because y-coordinates go from bottom to top
		float yOffset = lastY - yPos;
		lastX = xPos;
		lastY = yPos;
		camera.ProcessMouseInput(xOffset, yOffset);
	}

	void Renderer::MouseScrolledCallback(GLFWwindow *window, double xOffset, double yOffset)
	{
		camera.ProcessMouseScrollInput(yOffset);
	}
}