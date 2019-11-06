#pragma once

#include "Gogaman/Input/AbstractInput.h"

namespace Gogaman
{
	GM_CLASS_IMPLEMENTATION(WindowsInput, AbstractInput)
	{
	public:
		//Keyboard
		static bool IsKeyPressed(const int scancode);
		//Mouse
		static bool IsMouseButtonPressed(const int scancode);
		static const glm::vec2 &GetMousePosition();
	private:
		WindowsInput()  = delete;
		~WindowsInput() = delete;
	};
}