#pragma once

#include "Gogaman/Core.h"
#include "Gogaman/Input.h"

namespace Gogaman
{
	class GOGAMAN_API WindowsInput : public Input
	{
	public:
		WindowsInput();
		~WindowsInput();

		//Keyboard
		virtual bool IsKeyPressedImplementation(const int scancode) override;
		//Mouse
		virtual bool IsMouseButtonPressedImplementation(const int scancode) override;
		virtual glm::vec2 GetMousePositionImplementation() override;
	};
}