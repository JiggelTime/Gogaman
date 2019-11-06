#pragma once

#include "Gogaman/Core/AbstractWindow.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

struct GLFWwindow;

namespace Gogaman
{
	GM_CLASS_IMPLEMENTATION(WindowsWindow, AbstractWindow)
	{
	public:
		WindowsWindow(const char *title, const uint16_t width, const uint16_t height);
		~WindowsWindow();

		void Update();

		inline constexpr GLFWwindow *GetNativeWindow() { return m_Window; }
		
		void EnableVerticalSynchronization();
		void DisableVerticalSynchronization();

		static void Shutdown();
	private:
		static void GLFW_ErrorCallback(const int error, const char *description);
	private:
		GLFWwindow *m_Window;

		std::unique_ptr<RHI::Device> m_Device;

		static bool s_IsGLFW_Initialized;
	};
}