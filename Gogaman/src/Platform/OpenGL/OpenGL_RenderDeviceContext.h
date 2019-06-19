#pragma once

#include "Gogaman/Graphics/AbstractRenderDeviceContext.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class Window;

	class RenderDeviceContext : public AbstractRenderDeviceContext<RenderDeviceContext>
	{
	public:
		RenderDeviceContext(Window *window);
		~RenderDeviceContext();

		void Initialize();

		inline void SwapBuffers() { glfwSwapBuffers(m_GLFW_Window); }
	private:
		GLFWwindow *m_GLFW_Window;
	};
}