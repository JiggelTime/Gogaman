#pragma once

#include "Gogaman/Core/AbstractTime.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	class Time : public AbstractTime<Time>
	{
	public:
		static inline const float GetTime() { return (float)glfwGetTime(); }
	};
}