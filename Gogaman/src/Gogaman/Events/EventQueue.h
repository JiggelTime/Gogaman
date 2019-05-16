#pragma once

#include "pch.h"
#include "Gogaman/Core.h"
#include "Event.h"
#include "EventListener.h"

namespace Gogaman
{
	class GOGAMAN_API EventDispatcher
	{
	public:
		EventDispatcher(Event &event)
			: m_Event(event)
		{}

		template<typename T>
		bool Dispatch(std::function<bool(T &)> &callback)
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

		void AddListener(EventListener &listener, Event &event);
		void RemoveListenerEvent(EventListener &listener, Event &event);
		void RemoveListener(EventListener &listener);
	private:
		static const int m_MaxPendingEvents;
		static std::vector<Event> m_PendingEvents;
	};
}