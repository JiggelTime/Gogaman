#include "pch.h"
#include "OpenGL_RenderingContext.h"

#include "Gogaman/Application.h"
#include "Gogaman/Window.h"

namespace Gogaman
{
	RenderingContext::RenderingContext(Window *window)
		: m_GLFW_Window(static_cast<GLFWwindow *>(window->GetNativeWindow()))
	{}

	RenderingContext::~RenderingContext()
	{}

	void RenderingContext::Initialize()
	{
		glfwMakeContextCurrent(m_GLFW_Window);

		//Initialize GLAD
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GM_ASSERT(gladStatus, "Failed to initialize GLAD");
		GM_LOG_CORE_INFO("Initialized OpenGL |");
		GM_LOG_CORE_INFO("                   | Version:  %s", glGetString(GL_VERSION));
		GM_LOG_CORE_INFO("                   | Vendor:   %s", glGetString(GL_VENDOR));
		GM_LOG_CORE_INFO("                   | Renderer: %s", glGetString(GL_RENDERER));
	}
}