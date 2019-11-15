#pragma once

#include "Gogaman/Core/AbstractTime.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	GM_STATIC_CLASS_IMPLEMENTATION(WindowsTime, AbstractTime)
	{
	public:
		static inline float GetTime() { return (float)glfwGetTime(); }
	private:
		WindowsTime()  = delete;
		~WindowsTime() = delete;
	};
}