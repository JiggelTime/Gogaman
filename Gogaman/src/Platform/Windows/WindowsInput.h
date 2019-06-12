#pragma once

#include "Gogaman/Input.h"

namespace Gogaman
{
	class WindowsInput : public Input
	{
	public:
		WindowsInput();
		~WindowsInput();

		//Keyboard
		virtual bool IsKeyPressedImplementation(const int scancode)         const override;
		//Mouse
		virtual bool IsMouseButtonPressedImplementation(const int scancode) const override;
		virtual glm::vec2 GetMousePositionImplementation()                  const override;
	};
}