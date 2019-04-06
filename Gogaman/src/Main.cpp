#include "pch.h"
#include <glad.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <AntTweakBar.h>

#include "Core.h"
#include "Config.h"
#include "graphics/JitterSequences.h"
#include "graphics/Framebuffers.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "graphics/Model.h"
#include "graphics/texture3D.h"
#include "graphics/Light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(std::vector<std::string> faces);

Config config;

//Initialize camera attributes
	Camera camera(glm::vec3(0.0f, 0.5f, 0.0f));
	const float cameraNearPlane = 0.1f, cameraFarPlane = 100.0f;
	float lastX = config.screenWidth / 2.0f, lastY = config.screenHeight / 2.0f;
	bool firstMouse = true;
	float exposure = 1.0f;

//Timing
	float deltaTime = 0.0f, lastFrame = 0.0f;
	unsigned int frameCounter = 0;

int main()
{
	float aspectRatio = float(config.screenWidth) / float(config.screenHeight);

	//Initialize and configure GLFW
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create GLFW window
		GLFWwindow *window = glfwCreateWindow(config.screenWidth, config.screenHeight, "Gogaman", NULL, NULL);
		if(window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
		}
		
		glfwMakeContextCurrent(window);
		//Vertical synchronization
		glfwSwapInterval(config.vSync ? 1 : 0);

	//Initialize GLAD
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

	//Initialize AntTweakBar
		TwInit(TW_OPENGL_CORE, NULL);
		TwWindowSize(config.screenWidth, config.screenHeight);
		//Create tweak bar
		TwBar *tweakBar;
		tweakBar = TwNewBar("Debug GUI");

	//Set GLFW event callbacks
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Configure global OpenGL state
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_TEXTURE_3D);
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Build and compile shaders
		Shader precomputeBRDFShader("shaders/precomputeBRDF.vs", "shaders/precomputeBRDF.fs");
		Shader gbufferShader("shaders/gbuffershader.vs", "shaders/gbuffershader.fs");
		Shader downsampleNormalShader("shaders/downsampleNormal.vs", "shaders/downsampleNormal.fs");
		Shader downsampleDepthShader("shaders/downsampleDepth.vs", "shaders/downsampleDepth.fs");
		Shader voxelClearDynamicShader("shaders/voxelClearDynamic.compute");
		Shader voxelizationShader("shaders/voxelization.vs", "shaders/voxelization.fs", "shaders/voxelization.gs");
		Shader voxelInjectDirectShader("shaders/voxelInjectDirect.compute");
		Shader voxelInjectIndirectShader("shaders/voxelInjectIndirect.compute");
		Shader voxelConeTracingShader("shaders/voxelConeTracing.vs", "shaders/voxelConeTracing2.fs");
		Shader upsampleShader("shaders/upsample.vs", "shaders/upsample.fs");
		Shader directPBRShader("shaders/directPBR.vs", "shaders/directPBR.fs");
		Shader ssrShader("shaders/ssr.vs", "shaders/ssr.fs");
		Shader combineIndirectShader("shaders/combineIndirect.vs", "shaders/combineIndirect.fs");
		Shader skyboxShader("shaders/skyboxshader.vs", "shaders/skyboxshader.fs");
		Shader lampShader("shaders/lampshader.vs", "shaders/lampshader.fs");
		Shader taaShader("shaders/taa.vs", "shaders/taa.fs");
		Shader circleOfConfusionShader("shaders/coc.vs", "shaders/coc.fs");
		Shader circularBlurVerticalShader("shaders/circularBlurVertical.vs", "shaders/circularBlurVertical.fs");
		Shader circularBlurHorizontalShader("shaders/circularBlurHorizontal.vs", "shaders/circularBlurHorizontal.fs");
		Shader gaussianBlurShader("shaders/gaussianblurshader.vs", "shaders/gaussianblurshader.fs");
		Shader bloomShader("shaders/bloom.vs", "shaders/bloom.fs");
		Shader postProcessShader("shaders/postprocess.vs", "shaders/postprocess.fs");

	//Load models
		//Model blocksModel("D:/ProgrammingStuff/Resources/Models/Test_Scene/Blocks.obj");

		//Model planeModel("D:/ProgrammingStuff/Resources/Models/Plane_Tiled/Plane.obj");
		//Model zisBodyModel("D:/ProgrammingStuff/Resources/Models/zis101/zisBody.obj");
		//Model zisInstrumentsModel("D:/ProgrammingStuff/Resources/Models/zis101/zisInstruments.obj");
		//Model zisInteriorModel("D:/ProgrammingStuff/Resources/Models/zis101/zisInterior.obj");
		//Model zisLightsModel("D:/ProgrammingStuff/Resources/Models/zis101/zisLights.obj");
		//Model zisRoofModel("D:/ProgrammingStuff/Resources/Models/zis101/zisRoof.obj");
		//Model zisSteeringWheelModel("D:/ProgrammingStuff/Resources/Models/zis101/zisSteeringWheel.obj");
		//Model zisWheelsFrontModel("D:/ProgrammingStuff/Resources/Models/zis101/zisWheelsFront.obj");
		//Model zisWheelsRearModel("D:/ProgrammingStuff/Resources/Models/zis101/zisWheelsRear.obj");

		Model roomModel("D:/ProgrammingStuff/Resources/Models/Test_Scene/Room.obj");
		Model redModel("D:/ProgrammingStuff/Resources/Models/Test_Scene/Red.obj");
		Model blueModel("D:/ProgrammingStuff/Resources/Models/Test_Scene/Blue.obj");
		Model statueModel("D:/ProgrammingStuff/Resources/Models/Statue/Statue.obj");

		Model sphereModel("D:/ProgrammingStuff/Resources/Models/Sphere.obj");

	//Create framebuffers
		Framebuffers framebuffers(config);

	//Configure skybox
		float skyboxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,  1.0f };
		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		std::vector<std::string> faces
		{
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/right.jpg",
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/left.jpg",
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/top.jpg",
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/bottom.jpg",
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/front.jpg",
			"D:/ProgrammingStuff/Resources/Miscellaneous/skybox/back.jpg"
		};
		unsigned int cubemapTexture = loadCubemap(faces);

	//Configure quad
		float quadVertices[] = 
		{
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f
		};
		unsigned int quadVAO = 0, quadVBO;
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

	//Generate BRDF lookup texture
		unsigned int brdfLUT;
		glGenTextures(1, &brdfLUT);
		glBindTexture(GL_TEXTURE_2D, brdfLUT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.brdfFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, framebuffers.brdfRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

		glViewport(0, 0, 512, 512);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		precomputeBRDFShader.use();

		//Render full screen quad
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create voxel textures
		unsigned int voxelMaxMipLevels = glm::ceil(log2(config.voxelResolution) - 1);

		Texture3D *voxelAlbedo, *voxelNormal, *voxelEmissivity, *voxelDirectRadiance, *voxelTotalRadiance, *voxelStaticFlag;
		//Albedo
		voxelAlbedo         = new Texture3D(config.voxelResolution, config.voxelResolution, config.voxelResolution, true,  GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, voxelMaxMipLevels);
		//Normal and emissivity
		voxelNormal         = new Texture3D(config.voxelResolution, config.voxelResolution, config.voxelResolution, false, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 0, GL_NEAREST, GL_NEAREST);
		//Direct radiance
		voxelDirectRadiance = new Texture3D(config.voxelResolution, config.voxelResolution, config.voxelResolution, true,  GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, voxelMaxMipLevels);
		//Total radiance (direct + indirect)
		voxelTotalRadiance  = new Texture3D(config.voxelResolution, config.voxelResolution, config.voxelResolution, true,  GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, voxelMaxMipLevels);
		//Static flag
		voxelStaticFlag     = new Texture3D(config.voxelResolution, config.voxelResolution, config.voxelResolution, false, GL_R8,    GL_RED,  GL_UNSIGNED_BYTE, 0, GL_NEAREST, GL_NEAREST);

	unsigned int voxelizationCounter;

	//Set shader texture image units
		downsampleNormalShader.use();
		downsampleNormalShader.setInt("normalTexture", 0);

		downsampleDepthShader.use();
		downsampleDepthShader.setInt("depthTexture", 0);

		voxelClearDynamicShader.use();
		voxelClearDynamicShader.setInt("voxelAlbedo",     0);
		voxelClearDynamicShader.setInt("voxelNormal",     1);
		voxelClearDynamicShader.setInt("voxelStaticFlag", 2);

		voxelizationShader.use();
		voxelizationShader.setInt("voxelAlbedo",     0);
		voxelizationShader.setInt("voxelNormal",     1);
		voxelizationShader.setInt("voxelStaticFlag", 2);

		voxelInjectDirectShader.use();
		voxelInjectDirectShader.setInt("voxelAlbedo",         0);
		voxelInjectDirectShader.setInt("voxelNormal",         1);
		voxelInjectDirectShader.setInt("voxelDirectRadiance", 2);

		voxelInjectIndirectShader.use();
		voxelInjectIndirectShader.setInt("voxelAlbedo",         0);
		voxelInjectIndirectShader.setInt("voxelNormal",         1);
		voxelInjectIndirectShader.setInt("voxelDirectRadiance", 2);
		voxelInjectIndirectShader.setInt("voxelTotalRadiance",  3);

		voxelConeTracingShader.use();
		voxelConeTracingShader.setInt("gPositionMetalness",         1);
		voxelConeTracingShader.setInt("gNormal",                    2);
		voxelConeTracingShader.setInt("gAlbedoEmissivityRoughness", 3);

		upsampleShader.use();
		upsampleShader.setInt("inputTexture",         0);
		upsampleShader.setInt("previousInputTexture", 1);
		upsampleShader.setInt("depthTexture",         2);
		upsampleShader.setInt("depthCoarseTexture",   3);
		upsampleShader.setInt("normalTexture",        4);
		upsampleShader.setInt("normalCoarseTexture",  5);
		upsampleShader.setInt("velocityTexture",      6);

		directPBRShader.use();
		directPBRShader.setInt("gPositionMetalness",         0);
		directPBRShader.setInt("gNormal",                    1);
		directPBRShader.setInt("gAlbedoEmissivityRoughness", 2);
		directPBRShader.setInt("brdfLUT",                    3);
		directPBRShader.setInt("coneTracedDiffuse",          4);
		directPBRShader.setInt("coneTracedSpecular",         5);

		ssrShader.use();
		ssrShader.setInt("renderedImageTexture", 0);
		ssrShader.setInt("depthTexture",         1);
		ssrShader.setInt("normalTexture",        2);

		//combineIndirectShader.use();
		//combineIndirectShader.setInt("")
		//combineIndirectShader.setInt("")

		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);

		taaShader.use();
		taaShader.setInt("inputTexture",         0);
		taaShader.setInt("previousInputTexture", 1);
		taaShader.setInt("depthTexture",         2);
		taaShader.setInt("velocityTexture",      3);

		circleOfConfusionShader.use();
		circleOfConfusionShader.setInt("imageTexture", 0);
		circleOfConfusionShader.setInt("depthTexture", 1);

		circularBlurHorizontalShader.use();
		circularBlurHorizontalShader.setInt("cocTexture",   0);
		circularBlurHorizontalShader.setInt("imageTexture", 1);

		circularBlurVerticalShader.use();
		circularBlurVerticalShader.setInt("cocTexture",               0);
		circularBlurVerticalShader.setInt("imageRedChannelTexture",   1);
		circularBlurVerticalShader.setInt("imageGreenChannelTexture", 2);
		circularBlurVerticalShader.setInt("imageBlueChannelTexture",  3);

		gaussianBlurShader.use();
		gaussianBlurShader.setInt("imageTexture", 0);

		bloomShader.use();
		bloomShader.setInt("imageTexture", 0);
		bloomShader.setInt("bloomTexture", 1);

		postProcessShader.use();
		postProcessShader.setInt("hdrTexture", 0);

	bool firstIteration = true;

	//Variables for temporal sampling
		glm::vec2 temporalJitter = glm::vec2(0.0f);
		glm::vec2 previousTemporalJitter = temporalJitter;
		unsigned int temporalOffsetIterator = 0;
		glm::vec2 screenTexelSize = 1.0f / (glm::vec2(config.screenWidth, config.screenHeight) * config.resScale);
		//Jitter for cone traced global illumination
		glm::vec2 coneTraceJitter;
		unsigned int coneTraceJitterIterator = 0;

	//Initialize camera matrices
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		//Camera matrices from previous frame
		glm::mat4 previousViewProjectionMatrix;

	//Initialize OpenGL query timers
		//Time in ms
		float timeVCTGI = 0.0f, timeGeometryPass = 0.0f, timeSSR = 0.0f, timeDirectPBR = 0.0f, timeTAA = 0.0f, timeBloom = 0.0f, timeDOF = 0.0f;
		float previousTimeVCTGI = 0.0f, previousTimeGeometryPass = 0.0f, previousTimeSSR = 0.0f, previousTimeDirectPBR = 0.0f, previousTimeTAA = 0.0f, previousTimeBloom = 0.0f, previousTimeDOF = 0.0f;
		GLuint64 elapsedTime;
		GLint timerResultAvailable = 0;
		//Generate query
		GLuint query;
		glGenQueries(1, &query);

	//Render loop
	while(!glfwWindowShouldClose(window))
	{
		//Timing
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			frameCounter++;
		
		//Input
			processInput(window);

		//Update sub-pixel jitter for temporal resolve
			previousTemporalJitter = temporalJitter;
			temporalJitter         = (halton16[temporalOffsetIterator] - 0.5f) * screenTexelSize;
			temporalJitter        *= config.temporalJitterSpread;
			if(!config.taa)
				temporalJitter  = glm::vec2(0.0f);

			coneTraceJitter        = (halton16[temporalOffsetIterator] - 0.5f) * screenTexelSize;
			coneTraceJitter       *= 1.0f / config.giResScale;
			//if(!config.giUpscaling)
				coneTraceJitter = glm::vec2(0.0f);

			temporalOffsetIterator = temporalOffsetIterator >= 15 ? 0 : temporalOffsetIterator + 1;

			//Create translation matrix that translates by the jitter in x and y axis
			glm::mat4 jitterTranslationMatrix = glm::translate(glm::mat4(), glm::vec3(temporalJitter, 0.0f));

		//Update light(s)
			//Pointlight 0
			PointLight pointLight0;
			pointLight0.position = glm::vec3(0.4f, 1.2f, -0.6f);
			pointLight0.color    = glm::vec3(0.2f, 0.2f, 1.0f);
			//Pointlight 1
			PointLight pointLight1;
			pointLight1.position = glm::vec3(-0.4f, 1.2f, 0.6f);
			pointLight1.color    = glm::vec3(1.0f, 0.2f, 0.2f);

		//Update models
			statueModel.SetScale(0.4f);
			if(config.debug2) statueModel.SetPosition(glm::vec3(sin(glfwGetTime() * 2.0f) * 1.2f, 0.0f, 0.0f));

		//Update camera matrices
			previousViewProjectionMatrix = viewProjectionMatrix;
			projectionMatrix             = glm::perspective(glm::radians(camera.Zoom), aspectRatio, cameraNearPlane, cameraFarPlane);
			//Apply jitter
			projectionMatrix             = jitterTranslationMatrix * projectionMatrix;
			viewMatrix                   = camera.GetViewMatrix();
			viewProjectionMatrix         = projectionMatrix * viewMatrix;

		//Voxel camera
			//voxelGridPos = camera.Position;
			config.voxelGridSize *= 2.0f;
			auto voxelHalfGridSize = config.voxelGridSize / 2.0f;
			glm::mat4 voxelProjection = glm::ortho(-voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize, 0.0f, config.voxelGridSize);
			//if(debug) voxelProjection = glm::ortho(-voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize, -voxelHalfGridSize, voxelHalfGridSize);
			glm::mat4 voxelView = glm::lookAt(config.voxelGridPos, config.voxelGridPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 voxelViewProjection = voxelProjection * voxelView;
			config.voxelGridSize /= 2.0f;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//Set render mode to wireframe if enabled
			if(config.wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//Geometry pass
			//Begin geometry pass timer
				glBeginQuery(GL_TIME_ELAPSED, query);

			glEnable(GL_DEPTH_TEST);
			glViewport(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			gbufferShader.use();
			gbufferShader.setMat4("VP",                     viewProjectionMatrix);
			gbufferShader.setMat4("previousVP",             previousViewProjectionMatrix);
			gbufferShader.setVec2("temporalJitter",         temporalJitter);
			gbufferShader.setVec2("previousTemporalJitter", previousTemporalJitter);
			gbufferShader.setBool("normalMapping",          config.normalMapping);
			gbufferShader.setBool("emissive",               false);

			gbufferShader.setBool("debug",                  config.debug);

			//planeModel.Draw(gbufferShader, true);
			//zisBodyModel.Draw(gbufferShader, true);
			//zisInstrumentsModel.Draw(gbufferShader, true);
			//zisInteriorModel.Draw(gbufferShader, true);
			//zisLightsModel.Draw(gbufferShader, true);
			//zisRoofModel.Draw(gbufferShader, true);
			//zisSteeringWheelModel.Draw(gbufferShader, true);
			//zisWheelsFrontModel.Draw(gbufferShader, true);
			//zisWheelsRearModel.Draw(gbufferShader, true);

			roomModel.Draw(gbufferShader,   true);
			redModel.Draw(gbufferShader,    true);
			blueModel.Draw(gbufferShader,   true);
			statueModel.Draw(gbufferShader, true);

			glDisable(GL_DEPTH_TEST);

			//End geometry pass timer
				glEndQuery(GL_TIME_ELAPSED);
				//Wait until the query result are available
				timerResultAvailable = 0;
				while(!timerResultAvailable)
					glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

				//Get query result
				glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
				previousTimeGeometryPass = timeGeometryPass;
				timeGeometryPass = elapsedTime / 1000000.0f;

		//Downsample normal
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.normalDownsampleFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			downsampleNormalShader.use();
			downsampleNormalShader.setBool("debug", config.debug);

			int maxNormalMipLevels = floor(log2(std::max(config.screenWidth * config.resScale, config.screenHeight * config.resScale)));
			maxNormalMipLevels = 6;
			for (int i = 0; i < maxNormalMipLevels; i++)
			{
				int sampleMipLevel = i == 0 ? 0 : i - 1;
				int mipWidth = floor(config.screenWidth  * config.resScale * 0.5f * std::pow(0.5f, i));
				int mipHeight = floor(config.screenHeight * config.resScale * 0.5f * std::pow(0.5f, i));
				glViewport(0, 0, mipWidth, mipHeight);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers.normalDownsampleBuffer, i);
				glClear(GL_COLOR_BUFFER_BIT);

				downsampleNormalShader.setInt("sampleMipLevel", sampleMipLevel);

				glActiveTexture(GL_TEXTURE0);
				if (i == 0)
					glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.normalDownsampleBuffer);

				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
			}

		//Downsample depth
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.depthDownsampleFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			downsampleDepthShader.use();
			downsampleDepthShader.setBool("debug", config.debug);

			int maxDepthMipLevels = floor(log2(std::max(config.screenWidth * config.resScale, config.screenHeight * config.resScale)));
			maxDepthMipLevels = 6;
			for(int i = 0; i < maxDepthMipLevels; i++)
			{
				int sampleMipLevel = i == 0 ? 0 : i - 1;
				int mipWidth  = floor(config.screenWidth  * config.resScale * 0.5f * std::pow(0.5f, i));
				int mipHeight = floor(config.screenHeight * config.resScale * 0.5f * std::pow(0.5f, i));
				glViewport(0, 0, mipWidth, mipHeight);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers.depthDownsampleBuffer, i);
				glClear(GL_COLOR_BUFFER_BIT);

				downsampleDepthShader.setInt("sampleMipLevel", sampleMipLevel);

				glActiveTexture(GL_TEXTURE0);
				if(i == 0)
					glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.depthDownsampleBuffer);

				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
			}

		//Reset render mode if wireframe mode was enabled
			if(config.wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
		//Begin VCTGI timer
			glBeginQuery(GL_TIME_ELAPSED, query);

		//Voxelization
		if((config.autoVoxelize == true && voxelizationCounter >= config.voxelizationFrequency) || firstIteration == true)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
			glEnable(GL_CONSERVATIVE_RASTERIZATION_INTEL);

			glViewport(0, 0, config.voxelResolution, config.voxelResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Voxelize static models
			if(firstIteration == true)
			{
				voxelizationShader.use();
				voxelizationShader.setMat4("VP",                    voxelViewProjection);
				voxelizationShader.setInt("voxelResolution",        config.voxelResolution);
				voxelizationShader.setFloat("voxelGridSizeInverse", 1.0f / config.voxelGridSize);
				voxelizationShader.setVec3("voxelGridPos",          config.voxelGridPos);
				voxelizationShader.setBool("flagStatic",            true);
				voxelizationShader.setBool("emissive",              false);

				voxelizationShader.setBool("debug",                 config.debug);

				voxelAlbedo->Activate(voxelizationShader,     "voxelAlbedo",     0);
				voxelNormal->Activate(voxelizationShader,     "voxelNormal",     1);
				voxelStaticFlag->Activate(voxelizationShader, "voxelStaticFlag", 2);

				//Clear voxel textures
				glClearTexImage(voxelAlbedo->textureID,     0, GL_RGBA, GL_FLOAT, &clearColor);
				glClearTexImage(voxelNormal->textureID,     0, GL_RGBA, GL_FLOAT, &clearColor);
				glClearTexImage(voxelStaticFlag->textureID, 0, GL_RED,  GL_FLOAT, &clearColor[0]);

				//Bind voxel textures
				glBindImageTexture(0, voxelAlbedo->textureID,     0, GL_TRUE,  0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(1, voxelNormal->textureID,     0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
				glBindImageTexture(2, voxelStaticFlag->textureID, 0, GL_FALSE, 0, GL_READ_ONLY,  GL_R8);

				//Draw static models
				//planeModel.Draw(voxelizationShader);
				//zisBodyModel.Draw(voxelizationShader);
				//zisInstrumentsModel.Draw(voxelizationShader);
				//zisInteriorModel.Draw(voxelizationShader);
				//zisLightsModel.Draw(voxelizationShader);
				//zisRoofModel.Draw(voxelizationShader);
				//zisSteeringWheelModel.Draw(voxelizationShader);
				//zisWheelsFrontModel.Draw(voxelizationShader);
				//zisWheelsRearModel.Draw(voxelizationShader);

				roomModel.Draw(voxelizationShader);
				redModel.Draw(voxelizationShader);
				blueModel.Draw(voxelizationShader);

				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
			}

			//Clear dynamic voxels
			voxelClearDynamicShader.use();
			voxelClearDynamicShader.setInt("voxelResolution", config.voxelResolution);
			voxelAlbedo->Activate(voxelClearDynamicShader,     "voxelAlbedo",     0);
			voxelNormal->Activate(voxelClearDynamicShader,     "voxelNormal",     1);
			voxelStaticFlag->Activate(voxelClearDynamicShader, "voxelStaticFlag", 2);
			glBindImageTexture(0, voxelAlbedo->textureID, 0, GL_TRUE,  0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(1, voxelNormal->textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
			glDispatchCompute(config.voxelComputeWorkGroups, config.voxelComputeWorkGroups, config.voxelComputeWorkGroups);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

			//Render dynamic models
			voxelizationShader.use();
			voxelizationShader.setMat4("VP",                    voxelViewProjection);
			voxelizationShader.setInt("voxelResolution",        config.voxelResolution);
			voxelizationShader.setFloat("voxelGridSizeInverse", 1.0f / config.voxelGridSize);
			voxelizationShader.setVec3("voxelGridPos",          config.voxelGridPos);
			voxelizationShader.setBool("flagStatic",            false);

			//Bind voxel textures
			glBindImageTexture(0, voxelAlbedo->textureID,     0, GL_TRUE,  0, GL_READ_WRITE, GL_R32UI);
			glBindImageTexture(1, voxelNormal->textureID,     0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
			glBindImageTexture(0, voxelAlbedo->textureID, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(1, voxelNormal->textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
			glBindImageTexture(2, voxelStaticFlag->textureID, 0, GL_FALSE, 0, GL_READ_ONLY,  GL_R8);

			//Draw dynamic models
			statueModel.Draw(voxelizationShader);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_CULL_FACE);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
			glDisable(GL_CONSERVATIVE_RASTERIZATION_INTEL);

			voxelizationCounter = 0;
		}

		//Voxel direct light injection
			voxelInjectDirectShader.use();
			voxelInjectDirectShader.setVec3("pointLights[0].position",      pointLight0.position);
			voxelInjectDirectShader.setVec3("pointLights[0].color",         pointLight0.color);
			voxelInjectDirectShader.setFloat("pointLights[0].coneAperture", pointLight0.coneAperture);
			voxelInjectDirectShader.setVec3("pointLights[1].position",      pointLight1.position);
			voxelInjectDirectShader.setVec3("pointLights[1].color",         pointLight1.color);
			voxelInjectDirectShader.setFloat("pointLights[1].coneAperture", pointLight1.coneAperture);
			voxelInjectDirectShader.setInt("numLights",                     2);
			voxelInjectDirectShader.setInt("voxelResolution",               config.voxelResolution);
			voxelInjectDirectShader.setFloat("voxelGridSize",               config.voxelGridSize);
			voxelInjectDirectShader.setFloat("voxelGridSizeInverse",        1.0f / config.voxelGridSize);
			voxelInjectDirectShader.setVec3("voxelGridPos",                 config.voxelGridPos);

			voxelInjectDirectShader.setBool("debug",                        config.debug);

			voxelAlbedo->Activate(voxelInjectDirectShader,         "voxelAlbedo",         0);
			voxelNormal->Activate(voxelInjectDirectShader,         "voxelNormal",         1);
			voxelDirectRadiance->Activate(voxelInjectDirectShader, "voxelDirectRadiance", 2);
			glBindImageTexture(2, voxelDirectRadiance->textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			glClearTexImage(voxelDirectRadiance->textureID, 0, GL_RGBA, GL_FLOAT, &clearColor);

			glDispatchCompute(config.voxelComputeWorkGroups, config.voxelComputeWorkGroups, config.voxelComputeWorkGroups);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
			glGenerateTextureMipmap(voxelDirectRadiance->textureID);

		//Voxel indirect light injection
			voxelInjectIndirectShader.use();
			voxelInjectIndirectShader.setInt("voxelResolution",        config.voxelResolution);
			voxelInjectIndirectShader.setFloat("voxelGridSize",        config.voxelGridSize);
			voxelInjectIndirectShader.setFloat("voxelGridSizeInverse", 1.0f / config.voxelGridSize);
			voxelInjectIndirectShader.setVec3("voxelGridPos",          config.voxelGridPos);
			
			voxelInjectIndirectShader.setBool("debug",                 config.debug2);
			
			voxelAlbedo->Activate(voxelInjectIndirectShader,           "voxelAlbedo",         0);
			voxelNormal->Activate(voxelInjectIndirectShader,           "voxelNormal",         1);
			voxelDirectRadiance->Activate(voxelInjectIndirectShader,   "voxelDirectRadiance", 2);
			voxelTotalRadiance->Activate(voxelInjectIndirectShader,    "voxelTotalRadiance",  3);
			glBindImageTexture(3, voxelTotalRadiance->textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			glClearTexImage(voxelTotalRadiance->textureID, 0, GL_RGBA, GL_FLOAT, &clearColor);
			
			glDispatchCompute(config.voxelComputeWorkGroups, config.voxelComputeWorkGroups, config.voxelComputeWorkGroups);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
			glGenerateTextureMipmap(voxelTotalRadiance->textureID);

		//Screen space voxel Cone Tracing
			glViewport(0, 0, config.screenWidth * config.giResScale, config.screenHeight * config.giResScale);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.indirectFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			voxelConeTracingShader.use();
			voxelConeTracingShader.setVec3("pointLights[0].position", pointLight0.position);
			voxelConeTracingShader.setVec3("pointLights[0].color",    pointLight0.color);
			voxelConeTracingShader.setVec3("pointLights[1].position", pointLight1.position);
			voxelConeTracingShader.setVec3("pointLights[1].color",    pointLight1.color);
			voxelConeTracingShader.setInt("numLights",                2);
			voxelConeTracingShader.setInt("renderMode",               config.renderMode);
			voxelConeTracingShader.setFloat("voxelGridSize",          config.voxelGridSize);
			voxelConeTracingShader.setFloat("voxelGridSizeInverse",   1.0f / config.voxelGridSize);
			voxelConeTracingShader.setFloat("voxelWorldSize",         config.voxelGridSize / config.voxelResolution);
			voxelConeTracingShader.setVec3("voxelGridPos",            config.voxelGridPos);
			voxelConeTracingShader.setVec3("cameraPos",               camera.Position);
			voxelConeTracingShader.setVec2("coordJitter",             coneTraceJitter);

			voxelConeTracingShader.setBool("debug",                   config.debug);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gPositionMetalness);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gAlbedoEmissivityRoughness);
			voxelTotalRadiance->Activate(voxelConeTracingShader, "voxelTotalRadiance", 4);
			//Render full screen quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

			//End VCTGI timer
				glEndQuery(GL_TIME_ELAPSED);
				//Wait until the query result are available
				timerResultAvailable = 0;
				while(!timerResultAvailable)
					glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

				//Get query result
				glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
				previousTimeVCTGI = timeVCTGI;
				timeVCTGI = elapsedTime / 1000000.0f;

		//Upsample indirect lighting
			if(config.giUpscaling)
			{
				//Diffuse
				glViewport(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale);
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.upsampleFBO);
				glClear(GL_COLOR_BUFFER_BIT);

				upsampleShader.use();
				upsampleShader.setFloat("nearPlane",      cameraNearPlane);
				upsampleShader.setFloat("farPlane",       cameraFarPlane);
				upsampleShader.setInt("sampleTextureLod", std::max(floor(log2(1.0f / config.giResScale)) - 1.0f, 0.0f));

				upsampleShader.setBool("debug",           config.debug);
				upsampleShader.setBool("debug2",          config.debug2);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, framebuffers.indirectLightingBuffers[0]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, framebuffers.previousUpsampleBuffer);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				glActiveTexture(GL_TEXTURE3);
				if(config.giResScale < config.resScale)
					glBindTexture(GL_TEXTURE_2D, framebuffers.depthDownsampleBuffer);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
				glActiveTexture(GL_TEXTURE5);
				if(config.giResScale < config.resScale)
					glBindTexture(GL_TEXTURE_2D, framebuffers.normalDownsampleBuffer);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gVelocity);

				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				//Copy upscaled image to history buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.previousUpsampleFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.upsampleFBO);
				glBlitFramebuffer(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);

				//Specular
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.upsampleFBO2);
				glClear(GL_COLOR_BUFFER_BIT);

				upsampleShader.use();
				upsampleShader.setFloat("nearPlane",      cameraNearPlane);
				upsampleShader.setFloat("farPlane",       cameraFarPlane);
				upsampleShader.setInt("sampleTextureLod", std::max(floor(log2(1.0f / config.giResScale)) - 1.0f, 0.0f));

				upsampleShader.setBool("debug",           config.debug);
				upsampleShader.setBool("debug2",          config.debug2);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, framebuffers.indirectLightingBuffers[1]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, framebuffers.previousUpsampleBuffer2);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				glActiveTexture(GL_TEXTURE3);
				if(config.giResScale < config.resScale)
					glBindTexture(GL_TEXTURE_2D, framebuffers.depthDownsampleBuffer);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
				glActiveTexture(GL_TEXTURE5);
				if(config.giResScale < config.resScale)
					glBindTexture(GL_TEXTURE_2D, framebuffers.normalDownsampleBuffer);
				else
					glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gVelocity);

				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				//Copy upscaled image to history buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.previousUpsampleFBO2);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.upsampleFBO2);
				glBlitFramebuffer(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

		//Begin deferred shading timer
			glBeginQuery(GL_TIME_ELAPSED, query);

			//TODO: Render shadows at half-resolution (possibly quarter) stored in RGBA8 texture (possibly with 2 maps packed into each 8bit channel)
			//Do SSR with reprojected frame and VCT fallback. Upscale shadows, diffuse, and specular all in one pass, using MRT's to seperate results as needed.
			//Possibly store upsampled indirect radiance in one texture (chroma subsampling).

		//Deferred shading
			glViewport(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.hdrFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			directPBRShader.use();
			directPBRShader.setVec3("pointLights[0].position",      pointLight0.position);
			directPBRShader.setVec3("pointLights[0].color",         pointLight0.color);
			directPBRShader.setFloat("pointLights[0].coneAperture", pointLight0.coneAperture);
			directPBRShader.setVec3("pointLights[1].position",      pointLight1.position);
			directPBRShader.setVec3("pointLights[1].color",         pointLight1.color);
			directPBRShader.setFloat("pointLights[1].coneAperture", pointLight1.coneAperture);
			directPBRShader.setInt("numLights",                     2);
			directPBRShader.setVec3("cameraPos",                    camera.Position);
			directPBRShader.setFloat("voxelGridSize",               config.voxelGridSize);
			directPBRShader.setFloat("voxelGridSizeInverse",        1.0f / config.voxelGridSize);
			directPBRShader.setFloat("voxelWorldSize",              config.voxelGridSize / config.voxelResolution);
			directPBRShader.setVec3("voxelGridPos",                 config.voxelGridPos);

			directPBRShader.setInt("renderMode",                    config.renderMode);
			directPBRShader.setBool("debug",                        config.debug);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gPositionMetalness);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gAlbedoEmissivityRoughness);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, brdfLUT);
			glActiveTexture(GL_TEXTURE4);
			if(config.giUpscaling)
				glBindTexture(GL_TEXTURE_2D, framebuffers.upsampleBuffer);
			else
				glBindTexture(GL_TEXTURE_2D, framebuffers.indirectLightingBuffers[0]);
			glActiveTexture(GL_TEXTURE5);
			if(config.giUpscaling)
				glBindTexture(GL_TEXTURE_2D, framebuffers.upsampleBuffer2);
			else
				glBindTexture(GL_TEXTURE_2D, framebuffers.indirectLightingBuffers[1]);
			voxelTotalRadiance->Activate(directPBRShader, "voxelTexture", 6);

			//Render full screen quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

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
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.gBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.hdrFBO);
			glBlitFramebuffer(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.hdrFBO);
		
		//Forward rendering
			glEnable(GL_DEPTH_TEST);

			//Lights
			lampShader.use();
			lampShader.setMat4("projection", projectionMatrix);
			lampShader.setMat4("view", viewMatrix);
			//Light 0
			lampShader.setVec3("lightColor", pointLight0.color);
			sphereModel.SetPosition(pointLight0.position);
			sphereModel.SetScale(0.025f);
			sphereModel.Draw(lampShader);
			//Light 0
			lampShader.setVec3("lightColor", pointLight1.color);
			sphereModel.SetPosition(pointLight1.position);
			sphereModel.SetScale(0.025f);
			sphereModel.Draw(lampShader);
			/*
				//Skybox
			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			glm::mat4 cubemapView(glm::mat3(camera.GetViewMatrix()));
			skyboxShader.setMat4("projection", projectionMatrix);
			skyboxShader.setMat4("view", cubemapView);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glBindVertexArray(skyboxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); 
			*/
			glDisable(GL_DEPTH_TEST);
			/*
		//Screen space indirect specular
			ssrShader.use();
			ssrShader.setMat3("transposeInverseV", glm::transpose(glm::inverse(glm::mat3(viewMatrix))));
			ssrShader.setMat4("P",                 projectionMatrix);
			ssrShader.setMat4("inverseP",          glm::inverse(projectionMatrix));
			
			ssrShader.setBool("debug",             config.debug);
			ssrShader.setBool("debug2",            config.debug2);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gNormal);
			//Render full screen quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			
		//Combine indirect lighting
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.ssrFBO);
			glClear(GL_COLOR_BUFFER_BIT);

			combineIndirectShader.use();
			
			combineIndirectShader.setBool("debug", config.debug);
			combineIndirectShader.setBool("debug2", config.debug2);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, framebuffers.gAlbedoEmissivityRoughness);
			//Render full screen quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			*/

		//Begin temporal anti-aliasing timer
			glBeginQuery(GL_TIME_ELAPSED, query);

		//Temporal anti-aliasing
			if(config.taa)
			{
				taaShader.use();

				taaShader.setBool("debug",  config.debug);
				taaShader.setBool("debug2", config.debug2);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, framebuffers.previousFrameBuffer);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, framebuffers.gVelocity);
				//Render full screen quad
				glBindVertexArray(quadVAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);

				//Copy frame to history buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers.previousFrameFBO);
				glClear(GL_COLOR_BUFFER_BIT);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffers.hdrFBO);
				glBlitFramebuffer(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, 0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

		//End temporal anti-aliasing timer
			glEndQuery(GL_TIME_ELAPSED);
			//Wait until the query result are available
			timerResultAvailable = 0;
			while(!timerResultAvailable)
				glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &timerResultAvailable);

			//Get query result
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsedTime);
			previousTimeTAA = timeTAA;
			timeTAA = elapsedTime / 1000000.0f;

		//Depth of field
			if(config.dof)
			{
				//Begin depth of field timer
					glBeginQuery(GL_TIME_ELAPSED, query);

				glViewport(0, 0, config.screenWidth * config.dofResScale, config.screenHeight * config.dofResScale);

				//Compute circle of confusion
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.cocFBO);
					glClear(GL_COLOR_BUFFER_BIT);

					circleOfConfusionShader.use();
					circleOfConfusionShader.setFloat("nearPlane",     cameraNearPlane);
					circleOfConfusionShader.setFloat("farPlane",      cameraFarPlane);
					circleOfConfusionShader.setFloat("focalDistance", config.focalDistance);
					circleOfConfusionShader.setFloat("fStop",         config.fStop);
					circleOfConfusionShader.setFloat("focalLength",   config.focalLength);

					circleOfConfusionShader.setBool("debug",          config.debug);
					circleOfConfusionShader.setBool("debug2",         config.debug2);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, framebuffers.gDepth);
					//Render full screen quad
					glBindVertexArray(quadVAO);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glBindVertexArray(0);

				//Circular blur
					//Horizontal pass
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.circularBlurHorizontalFBO);
						glClear(GL_COLOR_BUFFER_BIT);

						circularBlurHorizontalShader.use();

						circularBlurHorizontalShader.setBool("debug", config.debug);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, framebuffers.cocBuffer);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
						//Render full screen quad
						glBindVertexArray(quadVAO);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
						glBindVertexArray(0);

					//Vertical pass and final composite
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.circularBlurVerticalFBO);
						glClear(GL_COLOR_BUFFER_BIT);

						circularBlurVerticalShader.use();

						circularBlurVerticalShader.setBool("debug", config.debug);
						
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, framebuffers.cocBuffer);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, framebuffers.circularBlurRedBuffer);
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, framebuffers.circularBlurGreenBuffer);
						glActiveTexture(GL_TEXTURE3);
						glBindTexture(GL_TEXTURE_2D, framebuffers.circularBlurBlueBuffer);
						//Render full screen quad
						glBindVertexArray(quadVAO);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
						glBindVertexArray(0);

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
			if(config.bloom)
			{
				//Begin bloom timer
					glBeginQuery(GL_TIME_ELAPSED, query);

				//Separable Gaussian blur
					bool horizontal = true, firstBlurIteration = true;
					//Blur radius is resolution independent
					float screenDiagonal = glm::sqrt((config.screenWidth * config.screenWidth) + (config.screenHeight * config.screenHeight));
					unsigned int bloomBlurIterations = glm::ceil(screenDiagonal * config.bloomBlurAmount);

					gaussianBlurShader.use();

					glViewport(0, 0, config.screenWidth * config.bloomResScale, config.screenHeight * config.bloomResScale);
					for(unsigned int i = 0; i < bloomBlurIterations; i++)
					{
						glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.pingpongFBO[horizontal]);
						if(firstBlurIteration)
							glClear(GL_COLOR_BUFFER_BIT);
						gaussianBlurShader.setBool("horizontal", horizontal);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, firstBlurIteration ? framebuffers.colorBuffers[1] : framebuffers.pingpongColorbuffers[!horizontal]);
						//Render full screen quad
						glBindVertexArray(quadVAO);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
						glBindVertexArray(0);
						horizontal = !horizontal;
						if(firstBlurIteration)
							firstBlurIteration = false;
					}

				//Apply bloom to image
					glViewport(0, 0, config.screenWidth * config.resScale, config.screenHeight * config.resScale);
					glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.hdrFBO);

					bloomShader.use();
					bloomShader.setFloat("bloomStrength", config.bloomStrength);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, framebuffers.pingpongColorbuffers[!horizontal]);
					//Render full screen quad
					glBindVertexArray(quadVAO);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glBindVertexArray(0);

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
			glViewport(0, 0, config.screenWidth, config.screenHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			postProcessShader.use();
			postProcessShader.setFloat("exposureBias", exposure);
			postProcessShader.setFloat("time",         glfwGetTime());

			postProcessShader.setBool("debug",         config.debug);

			glActiveTexture(GL_TEXTURE0);
			if(config.dof)
				glBindTexture(GL_TEXTURE_2D, framebuffers.circularBlurVerticalBuffer);
			else
				glBindTexture(GL_TEXTURE_2D, framebuffers.colorBuffers[0]);
			//Render full screen quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		
		//Print settings and timings
			std::cout << "Exposure: " << exposure << " | DOF: " << (config.dof ? "ON" : "OFF") << " | Bloom: " << (config.bloom ? "ON" : "OFF") << " | Normal mapping: " << (config.normalMapping ? "ON" : "OFF") << " | Debug: " << (config.debug ? "ON" : "OFF") << " | Debug 2: " << (config.debug2 ? "ON" : "OFF") << " | TAA: " << (config.taa ? "ON" : "OFF") << " | AutoVoxelize: " << config.autoVoxelize << std::endl;
			std::cout << "" << std::endl;
			std::cout << std::fixed << std::setprecision(2) << "Timings (ms); geometry pass: " << (timeGeometryPass + previousTimeGeometryPass) / 2.0f << ", VCTGI: " << (timeVCTGI + previousTimeVCTGI) / 2.0f << ", direct PBR: " << (timeDirectPBR + previousTimeDirectPBR) / 2.0f << ", TAA: " << (timeTAA + previousTimeTAA) / 2.0f << ", DOF: " << (timeDOF + previousTimeDOF) / 2.0f << ", bloom: " << (timeBloom + previousTimeBloom) / 2.0f << std::endl;
			std::cout << "" << std::endl;

		voxelizationCounter++;

		if(firstIteration == true)
			firstIteration = false;

		TwAddVarRO(tweakBar, "Cock Nigger", TW_TYPE_FLOAT, &config.screenWidth, NULL);

		//Render debug GUI
			TwDraw();

		//GLFW: Swap buffers and poll IO events
			glfwSwapBuffers(window);
			glfwPollEvents();
	}

	//Terminate AntTweakBar and GLFW
	TwTerminate();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) 
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
	if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !config.dof)
	{
		config.dof = !config.dof;
		config.dofKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
	{
		config.dofKeyPressed = false;
	}
	//Enable/disable bloom
	if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !config.bloomKeyPressed)
	{
		config.bloom = !config.bloom;
		config.bloomKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		config.bloomKeyPressed = false;
	}
	//Enable/disable debug mode
	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !config.debugKeyPressed)
	{
		config.debug = !config.debug;
		config.debugKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		config.debugKeyPressed = false;
	}
	//Enable/disable debug mode 2
	if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !config.debug2KeyPressed)
	{
		config.debug2 = !config.debug2;
		config.debug2KeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
	{
		config.debug2KeyPressed = false;
	}
	//Toggle spatio-temporal indirect lighting upscaling
	if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !config.giUpscalingKeyPressed)
	{
		config.giUpscaling = !config.giUpscaling;
		config.giUpscalingKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
	{
		config.giUpscalingKeyPressed = false;
	}
	//Toggle automatic revoxelization
	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !config.autoVoxelizeKeyPressed)
	{
		config.autoVoxelize = !config.autoVoxelize;
		config.autoVoxelizeKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		config.autoVoxelizeKeyPressed = false;
	}
	//Enable/disable normal mapping
	if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !config.normalMappingKeyPressed)
	{
		config.normalMapping = !config.normalMapping;
		config.normalMappingKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		config.normalMappingKeyPressed = false;
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
	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !config.wireframeKeyPressed)
	{
		config.wireframe = !config.wireframe;
		config.wireframeKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		config.wireframeKeyPressed = false;
	//Enable/disable temporal anti-aliasing
	if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !config.taaKeyPressed)
	{
		config.taa = !config.taa;
		config.taaKeyPressed = true;
	}
	if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		config.taaKeyPressed = false;
	}
	//Set render mode
	if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		config.renderMode = 0;
	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		config.renderMode = 1;
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		config.renderMode = 2;
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		config.renderMode = 3;
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		config.renderMode = 4;
	if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		config.renderMode = 5;
	if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		config.renderMode = 6;
	if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		config.renderMode = 7;
	if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		config.renderMode = 8;
}

//GLFW: This callbuck function executes whenever the window size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//GLFW: This callbuck function executes whenever the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	//Reversed because y-coordinates go from bottom to top
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseInput(xoffset, yoffset);
}

//GLFW: This callbuck function executes whenever the mouse scrolls
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
	camera.ProcessMouseScrollInput(yoffset);
}

//Loads cubemap
unsigned int loadCubemap(std::vector<std::string> faces) 
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for(GLuint i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if(data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}