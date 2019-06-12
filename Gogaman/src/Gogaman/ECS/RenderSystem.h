#pragma once

#include "System.h"
#include "RenderableComponent.h"

//OLD RENDERER INCLUDES
#include "Gogaman/Base.h"
#include "Gogaman/Config.h"
//#include "Gogaman/Events/EventListener.h"
//#include "Gogaman/Events/WindowEvent.h"
//#include "Gogaman/Events/KeyboardEvent.h"
//#include "Gogaman/Events/MouseEvent.h"
#include "Gogaman/Graphics/Camera.h"
#include "Platform/OpenGL/Texture2D.h"
#include "Platform/OpenGL/Texture3D.h"
#include "Platform/OpenGL/Renderbuffer.h"
#include "Platform/OpenGL/Framebuffer.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class World;

	class RenderSystem : public System
	{
	public:
		RenderSystem();

		virtual void Initialize() override;

		//virtual void Update()     override;
		virtual void Render()     override;

		GM_SET_SYSTEM_COMPONENTS(RenderableComponent)
	private:
		void InitializeFramebuffers();
		void RenderFullscreenQuad() const;
	private:
		//OLD RENDERER PRIVATE VARIABLES
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
		const unsigned int renderResWidth  = GM_CONFIG.screenWidth  * GM_CONFIG.resScale;
		const unsigned int renderResHeight = GM_CONFIG.screenHeight * GM_CONFIG.resScale;

		//Timing
		float deltaTime = 0.0f, lastFrame = 0.0f;
		unsigned int frameCounter = 0;

		std::unordered_map<std::string, Texture2D>    m_Texture2Ds;
		std::unordered_map<std::string, Renderbuffer> m_Renderbuffers;
		std::unordered_map<std::string, Framebuffer>  m_Framebuffers;

		//Fullscreen quad
		float quadVertices[20]{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
		unsigned int quadVAO = 0, quadVBO;

		//Camera matrices
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		//Camera matrix from previous frame
		glm::mat4 previousViewProjectionMatrix;
	};
}