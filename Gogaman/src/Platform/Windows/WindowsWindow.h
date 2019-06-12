#pragma once

#include "Gogaman/Window.h"

#include <glad.h>
#include <GLFW/glfw3.h>

namespace Gogaman
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const char *title, const int width, const int height);
		~WindowsWindow();

		virtual void Update() override;

		virtual void *GetNativeWindow() const override { return m_Window; }

		virtual void EnableVerticalSync()  override;
		virtual void DisableVerticalSync() override;
	private:
		static void GLFW_ErrorCallback(const int error, const char *description);
	private:
		GLFWwindow *m_Window;
		
		static bool s_GLFW_Initialized;
	};
}