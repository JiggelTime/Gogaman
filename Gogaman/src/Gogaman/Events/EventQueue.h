#pragma once

#include "pch.h"
#include "Gogaman/Core.h"
#include "Event.h"
#include "EventListener.h"

namespace Gogaman
{
	class GOGAMAN_API EventDispatcher
	{
		template<typename T>
		using EventCallback = std::function<bool(T &)>;
	public:
		EventDispatcher(Event &event)
			: m_Event(event)
		{}

		template<typename T>
		//bool Dispatch(std::function<bool(T &)> &callback)
		bool Dispatch(EventCallback<T> &callback)
		{
			if(m_Event.handled)
				return false;

			if(m_Event.GetType() == T::GetEventType())
			{
				m_Event.handled = callback(m_Event);
				return true;
			}

			return false;
		}
	private:
		Event &m_Event;
	};

	class GOGAMAN_API EventQueue
	{
	public:
		EventQueue();
		~EventQueue();

		void ProcessEvents();

		void AddListener(EventListener &listener, Event &event);
		void RemoveListenerEvent(EventListener &listener, Event &event);
		void RemoveListener(EventListener &listener);
	private:
		std::vector<EventListener> m_EventListeners;
		std::vector<Event> m_PendingEvents;
	};
}