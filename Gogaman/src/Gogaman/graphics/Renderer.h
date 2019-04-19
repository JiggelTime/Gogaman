#pragma once

#include "Gogaman/Core.h"

#include <glad.h>
#include <GLFW\glfw3.h>

namespace Gogaman
{
	class GOGAMAN_API Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Initialize();
		void Draw();

		void ProcessInput(GLFWwindow *window);
	};
}