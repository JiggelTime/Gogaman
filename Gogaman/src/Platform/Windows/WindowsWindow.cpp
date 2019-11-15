#include "pch.h"
#include "WindowsWindow.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"
#include "Gogaman/Events/EventManager.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	bool WindowsWindow::s_IsGLFW_Initialized = false;

	WindowsWindow::WindowsWindow(const char *title, const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		: AbstractWindow<WindowsWindow>(title, width, height, verticalSynchronization)
	{
		if(!s_IsGLFW_Initialized)
		{
			//Initialize GLFW
			glfwSetErrorCallback(GLFW_ErrorCallback);

			bool glfwSuccess = glfwInit();
			GM_ASSERT(glfwSuccess, "Failed to intitialize GLFW");

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);

			s_IsGLFW_Initialized = true;
		}
		
		m_Window = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title, nullptr, nullptr);
		GM_ASSERT(m_Window, "Failed to create GLFW window");

		m_Device = std::make_unique<RHI::Device>(m_Window);
		m_Device->CreateSwapChain(width, height, verticalSynchronization);

		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		//Set GLFW callback functions
		//Window
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) { WindowResizeEvent event(width, height); });

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) { WindowCloseEvent event; });

		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow *window, int entered)
		{
			if(entered)
				EventManager::GetInstance().Send(std::move(std::make_unique<WindowFocusEvent>()));
			else
				EventManager::GetInstance().Send(std::move(std::make_unique<WindowUnfocusEvent>()));
		});

		//Keyboard
		glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
		{
			if(action == GLFW_PRESS)
				EventManager::GetInstance().Send(std::move(std::make_unique<KeyPressEvent>(key, 0)));
			else if(action == GLFW_REPEAT)
				EventManager::GetInstance().Send(std::move(std::make_unique<KeyPressEvent>(key, 1)));
			else
				EventManager::GetInstance().Send(std::move(std::make_unique<KeyReleaseEvent>(key)));
		});

		//Mouse
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos) { EventManager::GetInstance().Send(std::move(std::make_unique<MouseMoveEvent>((float)xpos, (float)ypos))); });
		
		glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset) { EventManager::GetInstance().Send(std::move(std::make_unique<MouseScrollEvent>((float)xoffset, (float)yoffset))); });
		
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
		{
			if(action == GLFW_PRESS)
				EventManager::GetInstance().Send(std::move(std::make_unique<MouseButtonPressEvent>(button)));
			else
				EventManager::GetInstance().Send(std::move(std::make_unique<MouseButtonReleaseEvent>(button)));
		});
	}

	WindowsWindow::~WindowsWindow()
	{
		m_Device.reset();

		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::GLFW_ErrorCallback(const int error, const char *description)
	{
		GM_LOG_CORE_ERROR("GLFW error | Error code: %d | Message: %s", error, description);
	}
	
	void WindowsWindow::Update()
	{
		glfwPollEvents();
		//m_RenderingContext->SwapBuffers();
	}

	void WindowsWindow::Resize(const uint16_t width, const uint16_t height)
	{
		m_Device->RecreateSwapChain(width, height, m_VerticalSynchronization);

		m_Width  = width;
		m_Height = height;
	}

	void WindowsWindow::SetVerticalSynchronization(const VerticalSynchronization verticalSynchronization)
	{
		m_Device->RecreateSwapChain(m_Width, m_Height, verticalSynchronization);
		m_VerticalSynchronization = verticalSynchronization;
	}

	void WindowsWindow::Shutdown()
	{
		glfwTerminate();
	}
}