#pragma once

#include "Gogaman/Input/AbstractInput.h"

namespace Gogaman
{
	class Input : public AbstractInput<Input>
	{
	public:
		//Keyboard
		static bool IsKeyPressed(const int scancode);
		//Mouse
		static bool IsMouseButtonPressed(const int scancode);
		static const glm::vec2 &GetMousePosition();
	};
}