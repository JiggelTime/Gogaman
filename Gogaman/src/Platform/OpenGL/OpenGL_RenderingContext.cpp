#include "pch.h"
#include "OpenGL_RenderingContext.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Logging/Log.h"

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
		GM_ASSERT(gladStatus, "Failed to initialize GLAD")
		GM_LOG_CORE_INFO("Initialized OpenGL |");
		GM_LOG_CORE_INFO("                   | Version:  %s", glGetString(GL_VERSION));
		GM_LOG_CORE_INFO("                   | Vendor:   %s", glGetString(GL_VENDOR));
		GM_LOG_CORE_INFO("                   | Renderer: %s", glGetString(GL_RENDERER));

		//Initialize pipeline state
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		SetBlendState(BlendFactor::SourceAlpha, BlendFactor::InverseSourceAlpha);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//Debug callback
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) { GM_ASSERT(false, "OpenGL error | Type: 0x%x | Severity: 0x%x | Message: %s", type, severity, message) }, 0);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
}