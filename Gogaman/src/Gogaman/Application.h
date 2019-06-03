#pragma once

#include "Base.h"

#include "Window.h"
#include "Events/Event.h"

namespace Gogaman
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline Window &GetWindow() { return *m_Window; }

		inline static Application &GetInstance() { return *s_Instance; }
	private:
		bool m_IsRunning;

		std::unique_ptr<Window> m_Window;

		static Application *s_Instance;
	};

	Application *CreateApplication();
}