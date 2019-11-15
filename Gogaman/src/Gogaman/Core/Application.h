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

		inline const Window &GetWindow() const { return *m_Window.get(); }
		inline       Window &GetWindow()       { return *m_Window.get(); }

		inline constexpr const World &GetWorld() const { return m_World; }
		inline constexpr       World &GetWorld()       { return m_World; }

		static inline constexpr Application &GetInstance() { return *s_Instance; }
	private:
		bool m_IsRunning;

		std::unique_ptr<Window> m_Window;
		World                   m_World;

		static Application *s_Instance;
	};

	//Defined by client
	Application *CreateApplication();
}