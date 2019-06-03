#pragma once

#include "Gogaman/Base.h"
#include "Gogaman/Config.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Gogaman/Window.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Gogaman/Graphics/Camera.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "Renderbuffer.h"
#include "Framebuffer.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//#include <AntTweakBar.h>
#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class Renderer : public EventListener
	{
	public:
		Renderer(Window &window);
		~Renderer();

		void Render();

		virtual void OnEvent(Event &event) override;
	private:
		void RenderFullscreenQuad() const;
		
		void InitializeFramebuffers();

		bool OnWindowResize(WindowResizeEvent &event);
		bool OnMouseMove(MouseMoveEvent       &event);
		bool OnMouseScroll(MouseScrollEvent   &event);
		
		void PollInput(GLFWwindow *window);
	private:
		Window &m_Window;

		//TwBar *m_TweakBar;

		//Camera
		Camera camera = Camera(glm::vec3(0.0f, 0.5f, 0.0f));
		const float cameraNearPlane = 0.1f, cameraFarPlane = 100.0f;
		float exposure = 1.0f;

		float aspectRatio = float(GM_CONFIG.screenWidth) / float(GM_CONFIG.screenHeight);
		float lastX = GM_CONFIG.screenWidth / 2.0f, lastY = GM_CONFIG.screenHeight / 2.0f;
		bool firstMouse = true;
		bool firstIteration = true;

		//Window
		//GLFWwindow *m_Window;
		const uint renderResWidth       = GM_CONFIG.screenWidth  * GM_CONFIG.resScale;
		const uint renderResHeight      = GM_CONFIG.screenHeight * GM_CONFIG.resScale;

		const uint giRenderResWidth     = GM_CONFIG.screenWidth  * GM_CONFIG.giResScale;
		const uint giRenderResHeight    = GM_CONFIG.screenHeight * GM_CONFIG.giResScale;

		const uint dofRenderResWidth    = GM_CONFIG.screenWidth  * GM_CONFIG.dofResScale;
		const uint dofRenderResHeight   = GM_CONFIG.screenHeight * GM_CONFIG.dofResScale;

		const uint bloomRenderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.bloomResScale;
		const uint bloomRenderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.bloomResScale;

		//Timing
		float deltaTime = 0.0f, lastFrame = 0.0f;
		uint frameCounter = 0;

		std::unordered_map<std::string, Texture2D>    m_Texture2Ds;
		std::unordered_map<std::string, Renderbuffer> m_Renderbuffers;
		std::unordered_map<std::string, Framebuffer>  m_Framebuffers;

		//Fullscreen quad
		float quadVertices[20]{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
		uint quadVAO = 0, quadVBO;

		//VCTGI variables
		uint voxelizationCounter;
		//VCTGI textures
		Texture3D voxelAlbedo;
		Texture3D voxelNormal;
		Texture3D voxelDirectRadiance;
		Texture3D voxelTotalRadiance;
		Texture3D voxelStaticFlag;

		//TAA variables
		glm::vec2 temporalJitter = glm::vec2(0.0f);
		glm::vec2 previousTemporalJitter = temporalJitter;
		uint temporalOffsetIterator = 0;
		glm::vec2 screenTexelSize = 1.0f / (glm::vec2(GM_CONFIG.screenWidth, GM_CONFIG.screenHeight) * GM_CONFIG.resScale);
		glm::vec2 coneTraceJitter;
		uint coneTraceJitterIterator = 0;

		//Camera matrices
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		//Camera matrix from previous frame
		glm::mat4 previousViewProjectionMatrix;

		//OpenGL query timers
		//Time in ms
		float timeVCTGI = 0.0f, timeGeometryPass = 0.0f, timeSSR = 0.0f, timeDirectPBR = 0.0f, timeTAA = 0.0f, timeBloom = 0.0f, timeDOF = 0.0f;
		float previousTimeVCTGI = 0.0f, previousTimeGeometryPass = 0.0f, previousTimeSSR = 0.0f, previousTimeDirectPBR = 0.0f, previousTimeTAA = 0.0f, previousTimeBloom = 0.0f, previousTimeDOF = 0.0f;
		GLuint64 elapsedTime;
		GLint timerResultAvailable = 0;
		GLuint query;
	};
}