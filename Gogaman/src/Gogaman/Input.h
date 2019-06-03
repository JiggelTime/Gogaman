#pragma once

#include "Base.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class Input
	{
	public:
		//Keyboard
		inline static bool IsKeyPressed(const int scancode)         { return s_Instance->IsKeyPressedImplementation(scancode); }
		//Mouse
		inline static bool IsMouseButtonPressed(const int scancode) { return s_Instance->IsMouseButtonPressedImplementation(scancode); }
		inline static glm::vec2 GetMousePosition()                  { return s_Instance->GetMousePositionImplementation(); }
	protected:
		//Keyboard
		virtual bool IsKeyPressedImplementation(const int scancode)         const = 0;
		//Mouse
		virtual bool IsMouseButtonPressedImplementation(const int scancode) const = 0;
		virtual glm::vec2 GetMousePositionImplementation()                  const = 0;
	private:
		static Input *s_Instance;
	};
}