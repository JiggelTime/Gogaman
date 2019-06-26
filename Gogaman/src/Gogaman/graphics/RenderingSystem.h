#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/Config.h"
#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

#include "Camera.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "Texture2D.h"
#include "Platform/OpenGL/OpenGL_Renderbuffer.h"
#include "RenderSurface.h"
#include "FullscreenTriangle.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class World;

	class RenderingSystem : public System, public EventListener
	{
	public:
		RenderingSystem();

		virtual void Initialize() override;
		virtual void Update()     override;
		virtual void Render()     override;
		virtual void Shutdown()   override;

		virtual void OnEvent(Event &event) override;
	private:
		void InitializeRenderSurfaces();
		void InitializeShaders();

		void RenderFullscreenWindow() const { m_FullscreenTriangle->Render(); }

		bool OnWindowResize(WindowResizeEvent &event);
		bool OnMouseMove(MouseMoveEvent       &event);
		bool OnMouseScroll(MouseScrollEvent   &event);
	private:
		std::unique_ptr<ShaderManager> m_ShaderManager;
		ShaderID m_PrecomputeBRDFShader, m_GBufferShader, m_DeferredLightingShader, m_SkyboxShader, m_LightShader, m_PostprocessShader;

		std::unique_ptr<RenderSurface> m_BRDF_Buffer;
		Texture2D                      m_BRDF_LUT;
		Renderbuffer                   m_BRDF_Depth;

		std::unique_ptr<RenderSurface> m_G_Buffer;

		std::unique_ptr<RenderSurface> m_FinalBuffer;

		std::unique_ptr<FullscreenTriangle> m_FullscreenTriangle;
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
		int m_RenderResolutionWidth;
		int m_RenderResolutionHeight;
		const int m_BRDF_LUT_Width  = 512;
		const int m_BRDF_LUT_Height = 512;

		//Timing
		float deltaTime = 0.0f, lastFrame = 0.0f;
		uint32_t frameCounter = 0;

		std::unordered_map<std::string, Texture2D>    m_Texture2Ds;
		std::unordered_map<std::string, Renderbuffer> m_Renderbuffers;

		//Fullscreen quad
		//float quadVertices[20]{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f };
		//unsigned int quadVAO = 0, quadVBO;

		//Camera matrices
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 viewProjectionMatrix;
		//Camera matrix from previous frame
		glm::mat4 previousViewProjectionMatrix;
	};
}