#pragma once

#include "Gogaman/Core/AbstractTime.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	class WindowsTime : public AbstractTime<WindowsTime>
	{
	public:
		static inline float GetTime() { return (float)glfwGetTime(); }
	private:
		WindowsTime()  = delete;
		~WindowsTime() = delete;
	};
}