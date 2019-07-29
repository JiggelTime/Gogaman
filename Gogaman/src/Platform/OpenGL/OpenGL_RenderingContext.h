#pragma once

#include "Gogaman/Rendering/AbstractRenderingContext.h"

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

		inline void RenderIndexed(const uint32_t numIndices) const { glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0); }
	private:
		GLFWwindow *m_GLFW_Window;
	};
}