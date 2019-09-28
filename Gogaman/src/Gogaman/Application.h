#pragma once

#include "Window.h"
#include "ECS/World.h"

namespace Gogaman
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline Window &GetWindow() const { return *m_Window.get(); }
		inline World  &GetWorld()        { return  m_World;        }

		static inline Application &GetInstance() { return *s_Instance; }
	private:
		bool m_IsRunning;

		std::unique_ptr<Window> m_Window;
		World                   m_World;

		static Application *s_Instance;
	};

	Application *CreateApplication();
}

#define GM_RENDERING_CONTEXT Application::GetInstance().GetWindow().GetRenderingContext()