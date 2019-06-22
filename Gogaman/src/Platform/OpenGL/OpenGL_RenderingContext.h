#pragma once

#include "Gogaman/Graphics/AbstractRenderingContext.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class Window;

	class RenderingContext : public AbstractRenderingContext<RenderingContext>
	{
	public:
		RenderingContext(Window *window);
		~RenderingContext();

		void Initialize();

		inline void SwapBuffers() { glfwSwapBuffers(m_GLFW_Window); }
	private:
		GLFWwindow *m_GLFW_Window;
	};
}