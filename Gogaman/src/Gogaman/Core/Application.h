#pragma once

#include "Window.h"
#include "Gogaman/ECS/World.h"

namespace Gogaman
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline Window &GetWindow() { return *m_Window.get(); }

		inline constexpr World &GetWorld() { return m_World; }

		static inline Application &GetInstance() { return *s_Instance; }
	private:
		bool m_IsRunning;

		std::unique_ptr<Window> m_Window;
		World                   m_World;

		static Application *s_Instance;
	};

	//Defined by client
	Application *CreateApplication();
}