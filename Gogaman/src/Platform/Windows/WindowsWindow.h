#pragma once

#include "Gogaman/Core/AbstractWindow.h"

#include "Gogaman/Core/Base.h"

struct GLFWwindow;

namespace Gogaman
{
	class WindowsWindow : public AbstractWindow<WindowsWindow>
	{
	public:
		WindowsWindow(const char *title, const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization = VerticalSynchronization::Disabled);
		~WindowsWindow();

		void Update();

		void Resize(const uint16_t width, const uint16_t height);

		inline constexpr GLFWwindow *GetNativeWindow() { return m_Window; }
		
		void SetVerticalSynchronization(const VerticalSynchronization verticalSynchronization);

		static void Shutdown();
	private:
		static void GLFW_ErrorCallback(const int error, const char *description);
	private:
		GLFWwindow *m_Window;

		static bool s_IsGLFW_Initialized;
	};
}