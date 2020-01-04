#pragma once

#include "Window.h"
#include "Gogaman/ECS/World.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/EventDispatcher.h"
#include "Gogaman/Events/WindowEvent.h"

namespace Gogaman
{
	class Application : public EventListener
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		inline virtual void OnEvent(Event &event) override
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<WindowCloseEvent>(GM_BIND_EVENT_CALLBACK(Application::OnWindowClose));
		}

		inline const Window &GetWindow() const { return *m_Window.get(); }
		inline       Window &GetWindow()       { return *m_Window.get(); }

		inline constexpr const World &GetWorld() const { return m_World; }
		inline constexpr       World &GetWorld()       { return m_World; }

		static inline constexpr Application &GetInstance() { return *s_Instance; }
	private:
		inline bool OnWindowClose(WindowCloseEvent &event)
		{
			m_IsRunning = false;
			return true;
		}
	private:
		bool m_IsRunning;

		std::unique_ptr<Window> m_Window;
		World                   m_World;

		static Application *s_Instance;
	};

	//Defined by client
	Application *CreateApplication();
}