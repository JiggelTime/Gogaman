#include "pch.h"
#include "Renderer.h"

#include "Gogaman/Application.h"
#include "Gogaman/Logging/Log.h"
#include "Gogaman/Input.h"
#include "Gogaman/InputDefines.h"
#include "Gogaman/Events/EventDispatcher.h"
#include "Gogaman/ResourceManager.h"
#include "Gogaman/Rendering/JitterSequences.h"
#include "Gogaman/Rendering/Lights/PointLight.h"

namespace Gogaman
{
	Renderer::Renderer()
	{
		//Initialize GLAD
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GM_ASSERT(gladStatus, "Failed to initialize GLAD");
		GM_LOG_CORE_INFO("Initialized OpenGL");
		GM_LOG_CORE_INFO("Vendor:   %s", glGetString(GL_VENDOR));
		GM_LOG_CORE_INFO("Version:  %s", glGetString(GL_VERSION));
		GM_LOG_CORE_INFO("Renderer: %s", glGetString(GL_RENDERER));
		/*
		//Initialize AntTweakBar
		int twStatus = TwInit(TW_OPENGL_CORE, nullptr);
		GM_ASSERT(twStatus, "Failed to initialize AntTweakBar");
		TwWindowSize(GM_CONFIG.screenWidth, GM_CONFIG.screenHeight);
		m_TweakBar = TwNewBar("Debug");

		TwAddVarRO(m_TweakBar, "Gort", TW_TYPE_FLOAT, &GM_CONFIG.screenWidth, nullptr);
		*/
		//Initialize framebuffers
		InitializeFramebuffers();

		//Voxel textures
		voxelAlbedo.formatInternal = GL_RGBA8;
		voxelAlbedo.formatImage    = GL_RGBA;
		voxelAlbedo.filterMin      = GL_LINEAR_MIPMAP_LINEAR;
		voxelAlbedo.filterMag      = GL_LINEAR;
		voxelAlbedo.levels         = 0;
		voxelAlbedo.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelNormal.formatInternal = GL_RGBA8;
		voxelNormal.formatImage    = GL_RGBA;
		voxelNormal.filterMin      = GL_NEAREST;
		voxelNormal.filterMag      = GL_NEAREST;
		voxelNormal.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelDirectRadiance.formatInternal = GL_RGBA8;
		voxelDirectRadiance.formatImage    = GL_RGBA;
		voxelDirectRadiance.filterMin      = GL_LINEAR_MIPMAP_LINEAR;
		voxelDirectRadiance.filterMag      = GL_LINEAR;
		voxelDirectRadiance.levels         = 0;
		voxelDirectRadiance.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelTotalRadiance.formatInternal = GL_RGBA8;
		voxelTotalRadiance.formatImage    = GL_RGBA;
		voxelTotalRadiance.filterMin      = GL_LINEAR_MIPMAP_LINEAR;
		voxelTotalRadiance.filterMag      = GL_LINEAR;
		voxelTotalRadiance.levels         = 0;
		voxelTotalRadiance.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		voxelStaticFlag.formatInternal = GL_R8;
		voxelStaticFlag.formatImage    = GL_RED;
		voxelStaticFlag.filterMin      = GL_NEAREST;
		voxelStaticFlag.filterMag      = GL_NEAREST;
		voxelStaticFlag.Generate(GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution, GM_CONFIG.voxelResolution);

		//Shaders
		ResourceManager::LoadShader("precomputeBRDFShader", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.vs", "D:/dev/Gogaman/Gogaman/shaders/precomputeBRDF.fs");
		ResourceManager::LoadShader("gbufferShader", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.vs", "D:/dev/Gogaman/Gogaman/shaders/gbuffershader.fs");
		ResourceManager::LoadShader("downsampleNormalShader", "D:/dev/Gogaman/Gogaman/shaders/downsampleNormal.vs", "D:/dev/Gogaman/Gogaman/shaders/downsampleNormal.fs");
		ResourceManager::LoadShader("downsampleDepthShader", "D:/dev/Gogaman/Gogaman/shaders/downsampleDepth.vs", "D:/dev/Gogaman/Gogaman/shaders/downsampleDepth.fs");
		ResourceManager::LoadShader("voxelClearDynamicShader", "D:/dev/Gogaman/Gogaman/shaders/voxelClearDynamic.compute");
		ResourceManager::LoadShader("voxelizationShader", "D:/dev/Gogaman/Gogaman/shaders/voxelization.vs", "D:/dev/Gogaman/Gogaman/shaders/voxelization.fs", "D:/dev/Gogaman/Gogaman/shaders/voxelization.gs");
		ResourceManager::LoadShader("voxelInjectDirectShader", "D:/dev/Gogaman/Gogaman/shaders/voxelInjectDirect.compute");
		ResourceManager::LoadShader("voxelInjectIndirectShader", "D:/dev/Gogaman/Gogaman/shaders/voxelInjectIndirect.compute");
		ResourceManager::LoadShader("voxelConeTracingShader", "D:/dev/Gogaman/Gogaman/shaders/voxelConeTracing.vs", "D:/dev/Gogaman/Gogaman/shaders/voxelConeTracing2.fs");
		ResourceManager::LoadShader("upsampleIndirectShader", "D:/dev/Gogaman/Gogaman/shaders/upsampleIndirect.vs", "D:/dev/Gogaman/Gogaman/shaders/upsampleIndirect.fs");
		ResourceManager::LoadShader("directPBRShader", "D:/dev/Gogaman/Gogaman/shaders/directPBR.vs", "D:/dev/Gogaman/Gogaman/shaders/directPBR.fs");
		ResourceManager::LoadShader("ssrShader", "D:/dev/Gogaman/Gogaman/shaders/ssr.vs", "D:/dev/Gogaman/Gogaman/shaders/ssr.fs");
		//ResourceManager::LoadShader("combineIndirectShader", "D:/dev/Gogaman/Gogaman/shaders/combineIndirect.vs", "D:/dev/Gogaman/Gogaman/shaders/combineIndirect.fs");
		ResourceManager::LoadShader("skyboxShader", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.vs", "D:/dev/Gogaman/Gogaman/shaders/skyboxshader.fs");
		ResourceManager::LoadShader("lampShader", "D:/dev/Gogaman/Gogaman/shaders/lampshader.vs", "D:/dev/Gogaman/Gogaman/shaders/lampshader.fs");
		ResourceManager::LoadShader("taaShader", "D:/dev/Gogaman/Gogaman/shaders/taa.vs", "D:/dev/Gogaman/Gogaman/shaders/taa.fs");
		ResourceManager::LoadShader("circleOfConfusionShader", "D:/dev/Gogaman/Gogaman/shaders/coc.vs", "D:/dev/Gogaman/Gogaman/shaders/coc.fs");
		ResourceManager::LoadShader("circularBlurVerticalShader", "D:/dev/Gogaman/Gogaman/shaders/circularBlurVertical.vs", "D:/dev/Gogaman/Gogaman/shaders/circularBlurVertical.fs");
		ResourceManager::LoadShader("circularBlurHorizontalShader", "D:/dev/Gogaman/Gogaman/shaders/circularBlurHorizontal.vs", "D:/dev/Gogaman/Gogaman/shaders/circularBlurHorizontal.fs");
		ResourceManager::LoadShader("gaussianBlurShader", "D:/dev/Gogaman/Gogaman/shaders/gaussianblurshader.vs", "D:/dev/Gogaman/Gogaman/shaders/gaussianblurshader.fs");
		ResourceManager::LoadShader("bloomShader", "D:/dev/Gogaman/Gogaman/shaders/bloom.vs", "D:/dev/Gogaman/Gogaman/shaders/bloom.fs");
		ResourceManager::LoadShader("postProcessShader", "D:/dev/Gogaman/Gogaman/shaders/postprocess.vs", "D:/dev/Gogaman/Gogaman/shaders/postprocess.fs");

		//Models
		ResourceManager::LoadModel("roomModel",   "D:/ProgrammingStuff/Resources/Models/Test_Scene/Room.obj");
		ResourceManager::LoadModel("redModel",    "D:/ProgrammingStuff/Resources/Models/Test_Scene/Red.obj");
		//ResourceManager::LoadModel("blueModel",   "D:/ProgrammingStuff/Resources/Models/Test_Scene/Blue.obj");
		ResourceManager::LoadModel("statueModel", "D:/ProgrammingStuff/Resources/Models/Statue/Statue.obj");
		ResourceManager::LoadModel("sphereModel", "D:/ProgrammingStuff/Resources/Models/Sphere.obj");
		//GM_MODEL(sphereModel).Hide();

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
		GM_SHADER(downsampleNormalShader).Bind();
		GM_SHADER(downsampleNormalShader).SetUniformInt("normalTexture", 0);

		GM_SHADER(downsampleDepthShader).Bind();
		GM_SHADER(downsampleDepthShader).SetUniformInt("depthTexture", 0);

		GM_SHADER(voxelClearDynamicShader).Bind();
		GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelAlbedo", 0);
		GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelNormal", 1);
		GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelStaticFlag", 2);

		GM_SHADER(voxelizationShader).Bind();
		GM_SHADER(voxelizationShader).SetUniformInt("voxelAlbedo", 0);
		GM_SHADER(voxelizationShader).SetUniformInt("voxelNormal", 1);
		GM_SHADER(voxelizationShader).SetUniformInt("voxelStaticFlag", 2);

		GM_SHADER(voxelInjectDirectShader).Bind();
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelAlbedo", 0);
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelNormal", 1);
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelDirectRadiance", 2);

		GM_SHADER(voxelInjectIndirectShader).Bind();
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelAlbedo", 0);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelNormal", 1);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelDirectRadiance", 2);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelTotalRadiance", 3);

		GM_SHADER(voxelConeTracingShader).Bind();
		GM_SHADER(voxelConeTracingShader).SetUniformInt("gPositionMetalness", 1);
		GM_SHADER(voxelConeTracingShader).SetUniformInt("gNormal", 2);
		GM_SHADER(voxelConeTracingShader).SetUniformInt("gAlbedoEmissivityRoughness", 3);

		GM_SHADER(upsampleIndirectShader).Bind();
		GM_SHADER(upsampleIndirectShader).SetUniformInt("indirectDiffuseTexture", 0);
		GM_SHADER(upsampleIndirectShader).SetUniformInt("indirectSpecularTexture", 1);
		GM_SHADER(upsampleIndirectShader).SetUniformInt("depthTexture", 2);
		GM_SHADER(upsampleIndirectShader).SetUniformInt("depthCoarseTexture", 3);
		GM_SHADER(upsampleIndirectShader).SetUniformInt("normalTexture", 4);
		GM_SHADER(upsampleIndirectShader).SetUniformInt("normalCoarseTexture", 5);

		GM_SHADER(directPBRShader).Bind();
		GM_SHADER(directPBRShader).SetUniformInt("gPositionMetalness", 0);
		GM_SHADER(directPBRShader).SetUniformInt("gNormal", 1);
		GM_SHADER(directPBRShader).SetUniformInt("gAlbedoEmissivityRoughness", 2);
		GM_SHADER(directPBRShader).SetUniformInt("BRDF_LUT", 3);
		GM_SHADER(directPBRShader).SetUniformInt("coneTracedDiffuse", 4);
		GM_SHADER(directPBRShader).SetUniformInt("coneTracedSpecular", 5);
		GM_SHADER(directPBRShader).SetUniformInt("voxelTexture", 6);

		GM_SHADER(ssrShader).Bind();
		GM_SHADER(ssrShader).SetUniformInt("renderedImageTexture", 0);
		GM_SHADER(ssrShader).SetUniformInt("depthTexture", 1);
		GM_SHADER(ssrShader).SetUniformInt("normalTexture", 2);

		//GM_SHADER(combineIndirectShader).Bind();
		//GM_SHADER(combineIndirectShader).SetUniformInt("")
		//GM_SHADER(combineIndirectShader).SetUniformInt("")

		GM_SHADER(skyboxShader).Bind();
		GM_SHADER(skyboxShader).SetUniformInt("skybox", 0);

		GM_SHADER(taaShader).Bind();
		GM_SHADER(taaShader).SetUniformInt("inputTexture", 0);
		GM_SHADER(taaShader).SetUniformInt("previousInputTexture", 1);
		GM_SHADER(taaShader).SetUniformInt("depthTexture", 2);
		GM_SHADER(taaShader).SetUniformInt("velocityTexture", 3);

		GM_SHADER(circleOfConfusionShader).Bind();
		GM_SHADER(circleOfConfusionShader).SetUniformInt("imageTexture", 0);
		GM_SHADER(circleOfConfusionShader).SetUniformInt("depthTexture", 1);

		GM_SHADER(circularBlurHorizontalShader).Bind();
		GM_SHADER(circularBlurHorizontalShader).SetUniformInt("cocTexture", 0);
		GM_SHADER(circularBlurHorizontalShader).SetUniformInt("imageTexture", 1);

		GM_SHADER(circularBlurVerticalShader).Bind();
		GM_SHADER(circularBlurVerticalShader).SetUniformInt("cocTexture", 0);
		GM_SHADER(circularBlurVerticalShader).SetUniformInt("imageRedChannelTexture", 1);
		GM_SHADER(circularBlurVerticalShader).SetUniformInt("imageGreenChannelTexture", 2);
		GM_SHADER(circularBlurVerticalShader).SetUniformInt("imageBlueChannelTexture", 3);

		GM_SHADER(gaussianBlurShader).Bind();
		GM_SHADER(gaussianBlurShader).SetUniformInt("imageTexture", 0);

		GM_SHADER(bloomShader).Bind();
		GM_SHADER(bloomShader).SetUniformInt("imageTexture", 0);
		GM_SHADER(bloomShader).SetUniformInt("bloomTexture", 1);

		GM_SHADER(postProcessShader).Bind();
		GM_SHADER(postProcessShader).SetUniformInt("hdrTexture", 0);

		//Generate query
		glGenQueries(1, &query);
	}

	Renderer::~Renderer()
	{}

	void Renderer::InitializeFramebuffers()
	{
		//Precomputed BRDF
		m_Texture2Ds["BRDF_LUT"].formatInternal = GL_RG16F;
		m_Texture2Ds["BRDF_LUT"].formatImage    = GL_RG;
		m_Texture2Ds["BRDF_LUT"].filterMin      = GL_LINEAR;
		m_Texture2Ds["BRDF_LUT"].filterMag      = GL_LINEAR;
		m_Texture2Ds["BRDF_LUT"].Generate(512, 512);
		m_Framebuffers["BRDF_LUT_FB"].AttachColorBuffer(m_Texture2Ds["BRDF_LUT"]);

		//G-Buffer
		m_Texture2Ds["gPositionMetalness"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["gPositionMetalness"].formatImage    = GL_RGBA;
		m_Texture2Ds["gPositionMetalness"].filterMin      = GL_NEAREST;
		m_Texture2Ds["gPositionMetalness"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gPositionMetalness"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["gBufferFB"].AttachColorBuffer(m_Texture2Ds["gPositionMetalness"]);
		m_Texture2Ds["gNormal"].formatInternal = GL_RG16F;
		m_Texture2Ds["gNormal"].formatImage    = GL_RG;
		m_Texture2Ds["gNormal"].filterMin      = GL_NEAREST;
		m_Texture2Ds["gNormal"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gNormal"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["gBufferFB"].AttachColorBuffer(m_Texture2Ds["gNormal"]);
		m_Texture2Ds["gAlbedoEmissivityRoughness"].formatInternal = GL_RGBA8;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].formatImage    = GL_RGBA;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].filterMin      = GL_LINEAR;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].filterMag      = GL_LINEAR;
		m_Texture2Ds["gAlbedoEmissivityRoughness"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["gBufferFB"].AttachColorBuffer(m_Texture2Ds["gAlbedoEmissivityRoughness"]);
		m_Texture2Ds["gVelocity"].formatInternal = GL_RG16F;
		m_Texture2Ds["gVelocity"].formatImage    = GL_RG;
		m_Texture2Ds["gVelocity"].filterMin      = GL_NEAREST;
		m_Texture2Ds["gVelocity"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gVelocity"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["gBufferFB"].AttachColorBuffer(m_Texture2Ds["gVelocity"]);
		m_Texture2Ds["gDepth"].formatInternal = GL_DEPTH_COMPONENT32;
		m_Texture2Ds["gDepth"].formatImage    = GL_DEPTH_COMPONENT;
		m_Texture2Ds["gDepth"].filterMin      = GL_NEAREST;
		m_Texture2Ds["gDepth"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gDepth"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["gBufferFB"].AttachDepthBuffer(m_Texture2Ds["gDepth"]);
		
		//G-buffer normal mipmap
		m_Texture2Ds["gNormalMipmap"].formatInternal = GL_RG16F;
		m_Texture2Ds["gNormalMipmap"].formatImage    = GL_RG;
		m_Texture2Ds["gNormalMipmap"].filterMin      = GL_NEAREST_MIPMAP_NEAREST;
		m_Texture2Ds["gNormalMipmap"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gNormalMipmap"].levels         = 0;
		m_Texture2Ds["gNormalMipmap"].Generate(renderResWidth * 0.5f, renderResHeight * 0.5f);
		m_Framebuffers["gNormalMipmapFB"].AttachColorBuffer(m_Texture2Ds["gNormalMipmap"]);

		//G-buffer depth mipmap
		m_Texture2Ds["gDepthMipmap"].formatInternal = GL_R32F;
		m_Texture2Ds["gDepthMipmap"].formatImage    = GL_RED;
		m_Texture2Ds["gDepthMipmap"].filterMin      = GL_NEAREST_MIPMAP_NEAREST;
		m_Texture2Ds["gDepthMipmap"].filterMag      = GL_NEAREST;
		m_Texture2Ds["gDepthMipmap"].levels         = 0;
		m_Texture2Ds["gDepthMipmap"].Generate(renderResWidth * 0.5f, renderResHeight * 0.5f);
		m_Framebuffers["gDepthMipmapFB"].AttachColorBuffer(m_Texture2Ds["gDepthMipmap"]);

		//Final image
		m_Texture2Ds["finalImage"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["finalImage"].formatImage    = GL_RGBA;
		m_Texture2Ds["finalImage"].filterMin      = GL_LINEAR;
		m_Texture2Ds["finalImage"].filterMag      = GL_LINEAR;
		m_Texture2Ds["finalImage"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["finalImageFB"].AttachColorBuffer(m_Texture2Ds["finalImage"]);
		m_Renderbuffers["finalImageDepth"].formatInternal = GL_DEPTH_COMPONENT32;
		m_Renderbuffers["finalImageDepth"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["finalImageFB"].AttachDepthBuffer(m_Renderbuffers["finalImageDepth"]);

		//Final image history
		m_Texture2Ds["finalImageHistory"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["finalImageHistory"].formatImage    = GL_RGBA;
		m_Texture2Ds["finalImageHistory"].filterMin      = GL_NEAREST;
		m_Texture2Ds["finalImageHistory"].filterMag      = GL_NEAREST;
		m_Texture2Ds["finalImageHistory"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["finalImageHistoryFB"].AttachColorBuffer(m_Texture2Ds["finalImageHistory"]);

		//VCTGI
		m_Texture2Ds["VCTGI_Diffuse"].formatInternal = GL_RGBA8;
		m_Texture2Ds["VCTGI_Diffuse"].formatImage    = GL_RGBA;
		m_Texture2Ds["VCTGI_Diffuse"].filterMin      = GL_LINEAR;
		m_Texture2Ds["VCTGI_Diffuse"].filterMag      = GL_LINEAR;
		m_Texture2Ds["VCTGI_Diffuse"].Generate(giRenderResWidth, giRenderResHeight);
		m_Framebuffers["VCTGI_FB"].AttachColorBuffer(m_Texture2Ds["VCTGI_Diffuse"]);
		m_Texture2Ds["VCTGI_Specular"].formatInternal = GL_RGBA8;
		m_Texture2Ds["VCTGI_Specular"].formatImage    = GL_RGBA;
		m_Texture2Ds["VCTGI_Specular"].filterMin      = GL_LINEAR;
		m_Texture2Ds["VCTGI_Specular"].filterMag      = GL_LINEAR;
		m_Texture2Ds["VCTGI_Specular"].Generate(giRenderResWidth, giRenderResHeight);
		m_Framebuffers["VCTGI_FB"].AttachColorBuffer(m_Texture2Ds["VCTGI_Specular"]);

		//Indirect lighting upsample
		m_Texture2Ds["VCTGI_DiffuseUpsample"].formatInternal = GL_RGBA8;
		m_Texture2Ds["VCTGI_DiffuseUpsample"].formatImage    = GL_RGBA;
		m_Texture2Ds["VCTGI_DiffuseUpsample"].filterMin      = GL_NEAREST;
		m_Texture2Ds["VCTGI_DiffuseUpsample"].filterMag      = GL_NEAREST;
		m_Texture2Ds["VCTGI_DiffuseUpsample"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["IndirectUpsampleFB"].AttachColorBuffer(m_Texture2Ds["VCTGI_DiffuseUpsample"]);
		m_Texture2Ds["VCTGI_SpecularUpsample"].formatInternal = GL_RGBA8;
		m_Texture2Ds["VCTGI_SpecularUpsample"].formatImage    = GL_RGBA;
		m_Texture2Ds["VCTGI_SpecularUpsample"].filterMin      = GL_NEAREST;
		m_Texture2Ds["VCTGI_SpecularUpsample"].filterMag      = GL_NEAREST;
		m_Texture2Ds["VCTGI_SpecularUpsample"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["IndirectUpsampleFB"].AttachColorBuffer(m_Texture2Ds["VCTGI_SpecularUpsample"]);

		//SSR
		m_Texture2Ds["SSR"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["SSR"].formatImage = GL_RGBA;
		m_Texture2Ds["SSR"].filterMin = GL_NEAREST;
		m_Texture2Ds["SSR"].filterMag = GL_NEAREST;
		m_Texture2Ds["SSR"].Generate(renderResWidth, renderResHeight);
		m_Framebuffers["SSR_FB"].AttachColorBuffer(m_Texture2Ds["SSR"]);

		//Depth of field circle of confusion
		m_Texture2Ds["DOF_COC"].formatInternal = GL_R16F;
		m_Texture2Ds["DOF_COC"].formatImage    = GL_RED;
		m_Texture2Ds["DOF_COC"].filterMin      = GL_NEAREST;
		m_Texture2Ds["DOF_COC"].filterMag      = GL_NEAREST;
		m_Texture2Ds["DOF_COC"].Generate(dofRenderResWidth, dofRenderResHeight);
		m_Framebuffers["DOF_COC_FB"].AttachColorBuffer(m_Texture2Ds["DOF_COC"]);

		//Depth of field separable circular blur horizontal pass
		m_Texture2Ds["DOF_BlurHorizontalRed"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["DOF_BlurHorizontalRed"].formatImage    = GL_RGBA;
		m_Texture2Ds["DOF_BlurHorizontalRed"].filterMin      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalRed"].filterMag      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalRed"].Generate(dofRenderResWidth, dofRenderResHeight);
		m_Framebuffers["DOF_BlurHorizontalFB"].AttachColorBuffer(m_Texture2Ds["DOF_BlurHorizontalRed"]);
		m_Texture2Ds["DOF_BlurHorizontalGreen"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["DOF_BlurHorizontalGreen"].formatImage    = GL_RGBA;
		m_Texture2Ds["DOF_BlurHorizontalGreen"].filterMin      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalGreen"].filterMag      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalGreen"].Generate(dofRenderResWidth, dofRenderResHeight);
		m_Framebuffers["DOF_BlurHorizontalFB"].AttachColorBuffer(m_Texture2Ds["DOF_BlurHorizontalGreen"]);
		m_Texture2Ds["DOF_BlurHorizontalBlue"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["DOF_BlurHorizontalBlue"].formatImage    = GL_RGBA;
		m_Texture2Ds["DOF_BlurHorizontalBlue"].filterMin      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalBlue"].filterMag      = GL_NEAREST;
		m_Texture2Ds["DOF_BlurHorizontalBlue"].Generate(dofRenderResWidth, dofRenderResHeight);
		m_Framebuffers["DOF_BlurHorizontalFB"].AttachColorBuffer(m_Texture2Ds["DOF_BlurHorizontalBlue"]);

		//Depth of field separable circular blur vertical pass
		m_Texture2Ds["DOF_BlurVertical"].formatInternal = GL_RGBA16F;
		m_Texture2Ds["DOF_BlurVertical"].formatImage    = GL_RGBA;
		m_Texture2Ds["DOF_BlurVertical"].filterMin      = GL_LINEAR;
		m_Texture2Ds["DOF_BlurVertical"].filterMag      = GL_LINEAR;
		m_Texture2Ds["DOF_BlurVertical"].Generate(dofRenderResWidth, dofRenderResHeight);
		m_Framebuffers["DOF_BlurVerticalFB"].AttachColorBuffer(m_Texture2Ds["DOF_BlurVertical"]);

		//Bloom separable Gaussian blur vertical pass
		m_Texture2Ds["bloomBlurVertical"].formatInternal = GL_RGB16F;
		m_Texture2Ds["bloomBlurVertical"].formatImage    = GL_RGB;
		m_Texture2Ds["bloomBlurVertical"].filterMin      = GL_LINEAR;
		m_Texture2Ds["bloomBlurVertical"].filterMag      = GL_LINEAR;
		m_Texture2Ds["bloomBlurVertical"].Generate(bloomRenderResWidth, bloomRenderResHeight);
		m_Framebuffers["bloomBlurVerticalFB"].AttachColorBuffer(m_Texture2Ds["bloomBlurVertical"]);

		//Bloom separable Gaussian blur horizontal pass
		m_Texture2Ds["bloomBlurHorizontal"].formatInternal = GL_RGB16F;
		m_Texture2Ds["bloomBlurHorizontal"].formatImage    = GL_RGB;
		m_Texture2Ds["bloomBlurHorizontal"].filterMin      = GL_LINEAR;
		m_Texture2Ds["bloomBlurHorizontal"].filterMag      = GL_LINEAR;
		m_Texture2Ds["bloomBlurHorizontal"].Generate(bloomRenderResWidth, bloomRenderResHeight);
		m_Framebuffers["bloomBlurHorizontalFB"].AttachColorBuffer(m_Texture2Ds["bloomBlurHorizontal"]);
	}

	void Renderer::Render()
	{
		//Timing
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameCounter++;

		//Input
		PollInput(static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow()));

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
		std::vector<PointLight> pointLights;
		//Pointlight 0
			Gogaman::PointLight pointLight0;
			pointLight0.SetPosition(glm::vec3(sin(glfwGetTime()), 1.2f, cos(glfwGetTime())));
			pointLights.push_back(pointLight0);
			//Luminous intensity (candela)
			pointLight0.SetColor(glm::vec3(2.0f, 2.0f, 2.0f));
		//Pointlight 1
			//Gogaman::PointLight pointLight1;
			//pointLight1.SetPosition(glm::vec3(-0.1f, 1.2f, -0.8f));
			//Luminous intensity (candela)
			//pointLight1.SetColor(glm::vec3(0.2f, 0.2f, 2.0f));
			//pointLights.push_back(pointLight1);

		//Update models
		GM_MODEL(statueModel).SetScale(0.4f);
		//GM_MODEL(statueModel).SetPosition(glm::vec3(sin(glfwGetTime() * 2.0f) * 1.2f, 0.0f, 0.0f));
		GM_MODEL(statueModel).Rotate(glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

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
		const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//Set render mode to wireframe if enabled
		if(GM_CONFIG.wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Geometry pass
			//Begin geometry pass timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
		m_Framebuffers["gBufferFB"].Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GM_SHADER(gbufferShader).Bind();
		GM_SHADER(gbufferShader).SetUniformMat4("VP", viewProjectionMatrix);
		GM_SHADER(gbufferShader).SetUniformMat4("previousVP", previousViewProjectionMatrix);
		GM_SHADER(gbufferShader).SetUniformVec2("temporalJitter", temporalJitter);
		GM_SHADER(gbufferShader).SetUniformVec2("previousTemporalJitter", previousTemporalJitter);
		GM_SHADER(gbufferShader).SetUniformBool("normalMapping", GM_CONFIG.normalMapping);
		GM_SHADER(gbufferShader).SetUniformBool("emissive", false);

		GM_SHADER(gbufferShader).SetUniformBool("debug", GM_CONFIG.debug);

		for(auto &i : ResourceManager::models)
		{
			//if(!i.second.IsHidden())
				//i.second.Render(GM_SHADER(gbufferShader), true);
		}

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
		
		//Build gNormal mip pyramid
		m_Framebuffers["gNormalMipmapFB"].Bind();
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
			m_Framebuffers["gNormalMipmapFB"].AttachColorBuffer(m_Texture2Ds["gNormalMipmap"], i + 1, 0, true);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(downsampleNormalShader).SetUniformInt("sampleMipLevel", sampleMipLevel);
			if(i == 0)
				m_Texture2Ds["gNormal"].BindTexture(0);
			else
				m_Texture2Ds["gNormalMipmap"].BindTexture(0);

			RenderFullscreenQuad();
		}

		//Build gDepth mip pyramid
		m_Framebuffers["gDepthMipmapFB"].Bind();
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
			m_Framebuffers["gDepthMipmapFB"].AttachColorBuffer(m_Texture2Ds["gDepthMipmap"], i + 1, 0, true);
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(downsampleDepthShader).SetUniformInt("sampleMipLevel", sampleMipLevel);
			if(i == 0)
				m_Texture2Ds["gDepth"].BindTexture(0);
			else
				m_Texture2Ds["gDepthMipmap"].BindTexture(0);

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
			if(firstIteration == true)
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
				voxelAlbedo.BindTexture(0);
				GM_SHADER(voxelizationShader).SetUniformInt("voxelNormal", 1);
				voxelNormal.BindTexture(1);
				GM_SHADER(voxelizationShader).SetUniformInt("voxelStaticFlag", 2);
				voxelStaticFlag.BindTexture(2);

				//Clear voxel textures
				voxelAlbedo.Clear();
				voxelNormal.Clear();
				voxelStaticFlag.Clear();

				//Bind voxel textures
				voxelAlbedo.BindImage(0, 1, GL_READ_WRITE, GL_R32UI);
				voxelNormal.BindImage(1, 1, GL_READ_WRITE, GL_R32UI);
				voxelStaticFlag.BindImage(2, 1, GL_READ_ONLY, GL_R8);

				//Draw static models
				//GM_MODEL(roomModel).Render(GM_SHADER(voxelizationShader));
				//GM_MODEL(redModel).Render(GM_SHADER(voxelizationShader));
				//GM_MODEL(blueModel).Render(GM_SHADER(voxelizationShader));
				for(auto &i : ResourceManager::models)
				{
					if(!i.second.IsHidden())
					{
						//if(!i.second.IsDynamic())
							//i.second.Render(GM_SHADER(voxelizationShader));
					}
				}
				
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
			}

			//Clear dynamic voxels
			GM_SHADER(voxelClearDynamicShader).Bind();
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);

			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelAlbedo", 0);
			voxelAlbedo.BindTexture(0);
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelNormal", 1);
			voxelNormal.BindTexture(1);
			GM_SHADER(voxelClearDynamicShader).SetUniformInt("voxelStaticFlag", 2);
			voxelStaticFlag.BindTexture(2);

			voxelAlbedo.BindImage(0, 1, GL_READ_WRITE, GL_RGBA8);
			voxelNormal.BindImage(1, 1, GL_WRITE_ONLY, GL_RGBA8);
			
			glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

			GM_SHADER(voxelizationShader).Bind();
			GM_SHADER(voxelizationShader).SetUniformMat4("VP", voxelViewProjection);
			GM_SHADER(voxelizationShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
			GM_SHADER(voxelizationShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
			GM_SHADER(voxelizationShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);
			GM_SHADER(voxelizationShader).SetUniformBool("flagStatic", false);

			voxelAlbedo.BindImage(0, 1, GL_READ_WRITE, GL_R32UI);
			voxelNormal.BindImage(1, 1, GL_READ_WRITE, GL_R32UI);
			voxelStaticFlag.BindImage(2, 1, GL_READ_ONLY, GL_R8);

			//Render dynamic models
			for(auto &i : ResourceManager::models)
			{
				//if(i.second.IsDynamic())
					//i.second.Render(GM_SHADER(voxelizationShader));
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
		//GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		//GM_SHADER(voxelInjectDirectShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		//GM_SHADER(voxelInjectDirectShader).SetUniformFloat("pointLights[1].coneAperture", pointLight1.GetConeAperture());
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("numLights", pointLights.size());
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectDirectShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectDirectShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);

		GM_SHADER(voxelInjectDirectShader).SetUniformBool("debug", GM_CONFIG.debug);

		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelAlbedo", 0);
		voxelAlbedo.BindTexture(0);
		GM_SHADER(voxelInjectDirectShader).SetUniformInt("voxelNormal", 1);
		voxelNormal.BindTexture(1);

		voxelDirectRadiance.BindImage(2, 1, GL_WRITE_ONLY, GL_RGBA8);
		voxelDirectRadiance.Clear();

		glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		voxelDirectRadiance.RegenerateMipmap();

		//Voxel indirect light injection
		GM_SHADER(voxelInjectIndirectShader).Bind();
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelResolution", GM_CONFIG.voxelResolution);
		GM_SHADER(voxelInjectIndirectShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectIndirectShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelInjectIndirectShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);

		GM_SHADER(voxelInjectIndirectShader).SetUniformBool("debug", GM_CONFIG.debug2);

		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelAlbedo", 0);
		voxelAlbedo.BindTexture(0);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelNormal", 1);
		voxelNormal.BindTexture(1);
		GM_SHADER(voxelInjectIndirectShader).SetUniformInt("voxelDirectRadiance", 2);
		voxelDirectRadiance.BindTexture(2);

		voxelTotalRadiance.BindImage(3, 1, GL_WRITE_ONLY, GL_RGBA8);
		voxelTotalRadiance.Clear();

		glDispatchCompute(GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups, GM_CONFIG.voxelComputeWorkGroups);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		voxelTotalRadiance.RegenerateMipmap();

		//Screen space voxel Cone Tracing
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.giResScale, GM_CONFIG.screenHeight * GM_CONFIG.giResScale);
		m_Framebuffers["VCTGI_FB"].Bind();
		glClear(GL_COLOR_BUFFER_BIT);

		GM_SHADER(voxelConeTracingShader).Bind();
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[0].position", pointLight0.GetPosition());
		//GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[0].color", pointLight0.GetColor());
		//GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		//GM_SHADER(voxelConeTracingShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		GM_SHADER(voxelConeTracingShader).SetUniformInt("numLights", pointLights.size());
		GM_SHADER(voxelConeTracingShader).SetUniformInt("renderMode", GM_CONFIG.renderMode);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelGridSize", GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(voxelConeTracingShader).SetUniformFloat("voxelWorldSize", GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("voxelGridPos", GM_CONFIG.voxelGridPos);
		GM_SHADER(voxelConeTracingShader).SetUniformVec3("cameraPos", camera.Position);
		GM_SHADER(voxelConeTracingShader).SetUniformVec2("coordJitter", coneTraceJitter);

		GM_SHADER(voxelConeTracingShader).SetUniformBool("debug", GM_CONFIG.debug);

		m_Texture2Ds["gPositionMetalness"].BindTexture(1);
		m_Texture2Ds["gNormal"].BindTexture(2);
		m_Texture2Ds["gAlbedoEmissivityRoughness"].BindTexture(3);

		GM_SHADER(voxelConeTracingShader).SetUniformInt("voxelTotalRadiance", 4);
		voxelTotalRadiance.BindTexture(4);
		
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
			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
			m_Framebuffers["IndirectUpsampleFB"].Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(upsampleIndirectShader).Bind();
			GM_SHADER(upsampleIndirectShader).SetUniformFloat("nearPlane", cameraNearPlane);
			GM_SHADER(upsampleIndirectShader).SetUniformFloat("farPlane", cameraFarPlane);
			GM_SHADER(upsampleIndirectShader).SetUniformInt("sampleTextureLod", std::max(floor(log2(1.0f / GM_CONFIG.giResScale)) - 1.0f, 0.0f));

			GM_SHADER(upsampleIndirectShader).SetUniformBool("debug",  GM_CONFIG.debug);
			GM_SHADER(upsampleIndirectShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			m_Texture2Ds["VCTGI_Diffuse"].BindTexture(0);
			m_Texture2Ds["VCTGI_Specular"].BindTexture(1);
			m_Texture2Ds["gDepth"].BindTexture(2);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				m_Texture2Ds["gDepthMipmap"].BindTexture(3);
			else
				m_Texture2Ds["gDepth"].BindTexture(3);
			m_Texture2Ds["gNormal"].BindTexture(4);
			if(GM_CONFIG.giResScale < GM_CONFIG.resScale)
				m_Texture2Ds["gNormalMipmap"].BindTexture(5);
			else
				m_Texture2Ds["gNormal"].BindTexture(5);

			RenderFullscreenQuad();
		}

		//Begin deferred shading timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		//TODO: Render shadows at half-resolution (possibly quarter) stored in RGBA8 texture (possibly with 2 maps packed into each 8bit channel)
		//Do SSR with reprojected frame and VCT fallback. Upscale shadows, diffuse, and specular all in one pass, using MRT's to store results.
		//Possibly store upsampled indirect radiance in one texture (chroma subsampling).

		//TODO: Optimization: change internal format of separable circular blur convolution textures from floating point

	//Deferred shading
		glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.resScale, GM_CONFIG.screenHeight * GM_CONFIG.resScale);
		m_Framebuffers["finalImageFB"].Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GM_SHADER(directPBRShader).Bind();
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[0].position", pointLight0.GetPosition());
		GM_SHADER(directPBRShader).SetUniformVec3("pointLights[0].color", pointLight0.GetColor());
		GM_SHADER(directPBRShader).SetUniformFloat("pointLights[0].coneAperture", pointLight0.GetConeAperture());
		//GM_SHADER(directPBRShader).SetUniformVec3("pointLights[1].position", pointLight1.GetPosition());
		//GM_SHADER(directPBRShader).SetUniformVec3("pointLights[1].color", pointLight1.GetColor());
		//GM_SHADER(directPBRShader).SetUniformFloat("pointLights[1].coneAperture", pointLight1.GetConeAperture());
		GM_SHADER(directPBRShader).SetUniformInt("numLights",              pointLights.size());
		GM_SHADER(directPBRShader).SetUniformVec3("cameraPos",             camera.Position);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelGridSize",        GM_CONFIG.voxelGridSize);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelGridSizeInverse", 1.0f / GM_CONFIG.voxelGridSize);
		GM_SHADER(directPBRShader).SetUniformFloat("voxelWorldSize",       GM_CONFIG.voxelGridSize / GM_CONFIG.voxelResolution);
		GM_SHADER(directPBRShader).SetUniformVec3("voxelGridPos",          GM_CONFIG.voxelGridPos);

		GM_SHADER(directPBRShader).SetUniformInt("renderMode",             GM_CONFIG.renderMode);
		GM_SHADER(directPBRShader).SetUniformBool("debug",                 GM_CONFIG.debug);
		GM_SHADER(directPBRShader).SetUniformBool("debug2",                GM_CONFIG.debug2);

		m_Texture2Ds["gPositionMetalness"].BindTexture(0);
		m_Texture2Ds["gNormal"].BindTexture(1);
		m_Texture2Ds["gAlbedoEmissivityRoughness"].BindTexture(2);
		m_Texture2Ds["BRDF_LUT"].BindTexture(3);
		if(GM_CONFIG.giUpscaling)
			m_Texture2Ds["VCTGI_DiffuseUpsample"].BindTexture(4);
		else
			m_Texture2Ds["VCTGI_Diffuse"].BindTexture(4);
		if(GM_CONFIG.giUpscaling)
			m_Texture2Ds["VCTGI_SpecularUpsample"].BindTexture(5);
		else
			m_Texture2Ds["VCTGI_Specular"].BindTexture(5);
		voxelTotalRadiance.BindTexture(6);

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
		m_Framebuffers["finalImageFB"].BlitDepthBuffer(m_Framebuffers["gBufferFB"], renderResWidth, renderResHeight, GL_NEAREST);
		m_Framebuffers["finalImageFB"].Bind();

		//Forward rendering
		glEnable(GL_DEPTH_TEST);

		//Lights
		//GM_SHADER(lampShader).Bind();
		//GM_SHADER(lampShader).SetUniformMat4("projection", projectionMatrix);
		//GM_SHADER(lampShader).SetUniformMat4("view", viewMatrix);
		//Light 0
		//GM_SHADER(lampShader).SetUniformVec3("lightColor", pointLight0.GetColor());
		//GM_MODEL(sphereModel).SetPosition(pointLight0.GetPosition());
		//GM_MODEL(sphereModel).SetScale(0.025f);
		//GM_MODEL(sphereModel).Render(GM_SHADER(lampShader));
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

		if(GM_CONFIG.debug2)
			glDisable(GL_DEPTH_TEST);

		//Begin temporal anti-aliasing timer
		glBeginQuery(GL_TIME_ELAPSED, query);

		//Temporal anti-aliasing
		if(GM_CONFIG.taa)
		{
			GM_SHADER(taaShader).Bind();

			GM_SHADER(taaShader).SetUniformVec2("resolutionInput",  glm::vec2(m_Texture2Ds["finalImage"].width, m_Texture2Ds["finalImage"].height));
			GM_SHADER(taaShader).SetUniformVec2("resolutionOutput", glm::vec2(m_Texture2Ds["gVelocity"].width,  m_Texture2Ds["gVelocity"].height));
			GM_SHADER(taaShader).SetUniformVec2("texelSizeInput",   1.0f / glm::vec2(m_Texture2Ds["finalImage"].width, m_Texture2Ds["finalImage"].height));
			GM_SHADER(taaShader).SetUniformVec2("texelSizeOutput",  1.0f / glm::vec2(m_Texture2Ds["gVelocity"].width,  m_Texture2Ds["gVelocity"].height));

			GM_SHADER(taaShader).SetUniformBool("debug",            GM_CONFIG.debug);
			GM_SHADER(taaShader).SetUniformBool("debug2",           GM_CONFIG.debug2);

			m_Texture2Ds["finalImage"].BindTexture(0);
			m_Texture2Ds["finalImageHistory"].BindTexture(1);
			m_Texture2Ds["gDepth"].BindTexture(2);
			m_Texture2Ds["gVelocity"].BindTexture(3);
			
			RenderFullscreenQuad();

			//Copy frame to history buffer
			//m_Framebuffers["finalImageHistoryFB"].BlitColorBuffer(m_Framebuffers["finalImageFB"], renderResWidth, renderResHeight, GL_NEAREST);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffers["finalImageHistoryFB"].GetID());
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Framebuffers["finalImageFB"].GetID());
			glBlitFramebuffer(0, 0, renderResWidth, renderResHeight, 0, 0, renderResWidth, renderResHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
			m_Framebuffers["DOF_COC_FB"].Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circleOfConfusionShader).Bind();
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("nearPlane", cameraNearPlane);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("farPlane", cameraFarPlane);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("focalDistance", GM_CONFIG.focalDistance);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("fStop", GM_CONFIG.fStop);
			GM_SHADER(circleOfConfusionShader).SetUniformFloat("focalLength", GM_CONFIG.focalLength / 1000.0f);

			GM_SHADER(circleOfConfusionShader).SetUniformBool("debug", GM_CONFIG.debug);
			GM_SHADER(circleOfConfusionShader).SetUniformBool("debug2", GM_CONFIG.debug2);

			m_Texture2Ds["finalImage"].BindTexture(0);
			m_Texture2Ds["gDepth"].BindTexture(1);

			RenderFullscreenQuad();

			//Circular blur
				//Horizontal pass
			m_Framebuffers["DOF_BlurHorizontalFB"].Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circularBlurHorizontalShader).Bind();

			GM_SHADER(circularBlurHorizontalShader).SetUniformBool("debug", GM_CONFIG.debug);

			m_Texture2Ds["DOF_COC"].BindTexture(0);
			m_Texture2Ds["finalImage"].BindTexture(1);
			
			RenderFullscreenQuad();

			//Vertical pass and final composite
			m_Framebuffers["DOF_BlurVerticalFB"].Bind();
			glClear(GL_COLOR_BUFFER_BIT);

			GM_SHADER(circularBlurVerticalShader).Bind();

			GM_SHADER(circularBlurVerticalShader).SetUniformBool("debug", GM_CONFIG.debug);

			//Framebuffers::cocBuffer.BindTexture(0);
			m_Texture2Ds["DOF_COC"].BindTexture(0);
			m_Texture2Ds["DOF_BlurHorizontalRed"].BindTexture(1);
			m_Texture2Ds["DOF_BlurHorizontalGreen"].BindTexture(2);
			m_Texture2Ds["DOF_BlurHorizontalBlue"].BindTexture(3);
			
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
		//TODO: make uniform for firstBlurIteration and cut off < 1.0 luminance
		if(GM_CONFIG.bloom)
		{
			//Begin bloom timer
			glBeginQuery(GL_TIME_ELAPSED, query);

			//Separable Gaussian blur
			bool horizontal = true, firstBlurIteration = true;
			//Blur radius is resolution independent
			float screenDiagonal = glm::sqrt((GM_CONFIG.screenWidth * GM_CONFIG.screenWidth) + (GM_CONFIG.screenHeight * GM_CONFIG.screenHeight));
			int bloomBlurIterations = glm::ceil(screenDiagonal * GM_CONFIG.bloomBlurAmount);

			GM_SHADER(gaussianBlurShader).Bind();

			glViewport(0, 0, GM_CONFIG.screenWidth * GM_CONFIG.bloomResScale, GM_CONFIG.screenHeight * GM_CONFIG.bloomResScale);
			for(int i = 0; i < bloomBlurIterations; i++)
			{
				if(horizontal)
					m_Framebuffers["bloomBlurHorizontalFB"].Bind();
				else
					m_Framebuffers["bloomBlurVerticalFB"].Bind();
				if(firstBlurIteration)
					glClear(GL_COLOR_BUFFER_BIT);
				GM_SHADER(gaussianBlurShader).SetUniformBool("horizontal", horizontal);
				if(firstBlurIteration)
					m_Texture2Ds["finalImage"].BindTexture(0);
				else
				{
					if(horizontal)
						m_Texture2Ds["bloomBlurVertical"].BindTexture(0);
					else
						m_Texture2Ds["bloomBlurHorizontal"].BindTexture(0);
				}

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
			m_Framebuffers["finalImageFB"].Bind();

			GM_SHADER(bloomShader).Bind();
			GM_SHADER(bloomShader).SetUniformFloat("bloomStrength", GM_CONFIG.bloomStrength);

			if(GM_CONFIG.dof)
				m_Texture2Ds["DOF_BlurVertical"].BindTexture(0);
			else
				m_Texture2Ds["finalImage"].BindTexture(0);
			if(horizontal)
				m_Texture2Ds["bloomBlurVertical"].BindTexture(1);
			else
				m_Texture2Ds["bloomBlurHorizontal"].BindTexture(1);
			
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
		GM_SHADER(postProcessShader).SetUniformFloat("time", (float)glfwGetTime());

		GM_SHADER(postProcessShader).SetUniformBool("debug", GM_CONFIG.debug);

		if(!GM_CONFIG.bloom && GM_CONFIG.dof)
			m_Texture2Ds["DOF_BlurVertical"].BindTexture(0);
		else
			m_Texture2Ds["finalImage"].BindTexture(0);

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
		//TwDraw();
	}

	void Renderer::RenderFullscreenQuad() const
	{
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void Renderer::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>(GM_BIND_EVENT_CALLBACK(Renderer::OnWindowResize));

		dispatcher.Dispatch<MouseMoveEvent>(GM_BIND_EVENT_CALLBACK(Renderer::OnMouseMove));
		dispatcher.Dispatch<MouseScrollEvent>(GM_BIND_EVENT_CALLBACK(Renderer::OnMouseScroll));
	}

	bool Renderer::OnWindowResize(WindowResizeEvent &event)
	{
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return false;
	}

	bool Renderer::OnMouseMove(MouseMoveEvent &event)
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
		return false;
	}

	bool Renderer::OnMouseScroll(MouseScrollEvent &event)
	{
		camera.ProcessMouseScrollInput(event.GetOffsetY());
		return false;
	}

	void Renderer::PollInput(GLFWwindow *window)
	{
		
	}
}