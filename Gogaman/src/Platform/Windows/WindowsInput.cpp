#include "pch.h"
#include "WindowsInput.h"

#include "Gogaman/Application.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	Input *Input::s_Instance = new WindowsInput();

	WindowsInput::WindowsInput()
	{}

	WindowsInput::~WindowsInput()
	{}

	bool WindowsInput::IsKeyPressedImplementation(const int scancode) const
	{
		auto window = static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto keyState = glfwGetKey(window, scancode);
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImplementation(const int scancode) const
	{
		auto window = static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow());
		auto buttonState = glfwGetMouseButton(window, scancode);
		return buttonState == GLFW_PRESS;
	}

	glm::vec2 WindowsInput::GetMousePositionImplementation() const
	{
		auto window = static_cast<GLFWwindow *>(Application::GetInstance().GetWindow().GetNativeWindow());
		glm::dvec2 mousePosition;
		glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
		return mousePosition;
	}
}