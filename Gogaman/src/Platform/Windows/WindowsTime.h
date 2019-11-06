#pragma once

#include "Gogaman/Core/AbstractTime.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	GM_CLASS_IMPLEMENTATION(WindowsTime, AbstractTime)
	{
	public:
		static inline const float GetTime() { return (float)glfwGetTime(); }
	};
}