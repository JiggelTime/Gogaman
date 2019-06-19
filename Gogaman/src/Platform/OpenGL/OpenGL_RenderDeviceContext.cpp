#include "pch.h"
#include "OpenGL_RenderDeviceContext.h"

#include "Gogaman/Application.h"
#include "Gogaman/Window.h"

namespace Gogaman
{
	RenderDeviceContext::RenderDeviceContext(Window *window)
		: m_GLFW_Window(static_cast<GLFWwindow *>(window->GetNativeWindow()))
	{}

	RenderDeviceContext::~RenderDeviceContext()
	{}

	void RenderDeviceContext::Initialize()
	{
		glfwMakeContextCurrent(m_GLFW_Window);

		//Initialize GLAD
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GM_ASSERT(gladStatus, "Failed to initialize GLAD");
		GM_LOG_CORE_INFO("OpenGL");
		GM_LOG_CORE_INFO("| Version:  %s", glGetString(GL_VERSION));
		GM_LOG_CORE_INFO("| Vendor:   %s", glGetString(GL_VENDOR));
		GM_LOG_CORE_INFO("| Renderer: %s", glGetString(GL_RENDERER));
	}
}