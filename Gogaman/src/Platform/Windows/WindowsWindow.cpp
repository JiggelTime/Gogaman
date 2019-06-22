#include "pch.h"
#include "WindowsWindow.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"
#include "Gogaman/Events/EventManager.h"

namespace Gogaman
{
	bool WindowsWindow::s_GLFW_Initialized = false;

	WindowsWindow::WindowsWindow(const char *title, const int width, const int height)
		: Window(title, width, height)
	{
		if(!s_GLFW_Initialized)
		{
			//Initialize GLFW
			glfwSetErrorCallback(GLFW_ErrorCallback);

			bool glfwSuccess = glfwInit();
			GM_ASSERT(glfwSuccess, "Failed to intitialize GLFW");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			s_GLFW_Initialized = true;
		}
		
		//Create GLFW window
		m_Window = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title, nullptr, nullptr);
		GM_ASSERT(m_Window, "Failed to create GLFW window");

		m_RenderingContext = std::make_unique<RenderingContext>(this);
		m_RenderingContext->Initialize();

		EnableVerticalSync();
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		//Set GLFW event callbacks
		//Window
		glfwSetWindowSizeCallback(m_Window,  [](GLFWwindow *window, int width, int height) { WindowResizeEvent event(width, height); });
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
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos)    { EventManager::GetInstance().Send(std::move(std::make_unique<MouseMoveEvent>((float)xpos, (float)ypos)));         });
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
		if(m_Window != nullptr)
			glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::GLFW_ErrorCallback(const int error, const char *description)
	{
		GM_LOG_CORE_ERROR("GLFW error %d, %s", error, description);
	}

	Window *Window::Create(const char *title, const int width, const int height)
	{
		return new WindowsWindow(title, width, height);
	}
	
	void WindowsWindow::Update()
	{
		glfwPollEvents();
		m_RenderingContext->SwapBuffers();
	}

	void Window::Shutdown()
	{
		glfwTerminate();
	}

	void WindowsWindow::EnableVerticalSync()
	{
		glfwSwapInterval(1);
		m_VerticalSynchronization = true;
	}

	void WindowsWindow::DisableVerticalSync()
	{
		glfwSwapInterval(0);
		m_VerticalSynchronization = false;
	}
}