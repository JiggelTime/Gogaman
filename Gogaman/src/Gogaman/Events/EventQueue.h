#pragma once

#include "pch.h"
#include "Gogaman/Core.h"
#include "Event.h"
#include "EventListener.h"

namespace Gogaman
{
	class EventListener;

	class GOGAMAN_API EventQueue
	{
	public:
		static void Enqueue(Event &event);

		static void DispatchEvents();

		static void AddListener(EventListener &listener);
		static void RemoveListenerFromEvent(EventListener &listener, Event &event);
		static void RemoveListener(EventListener &listener);

		static inline void Clear() { m_PendingEvents = {}; }

		static inline size_t GetNumEventListeners() { return m_EventListeners.size(); }
		static inline size_t GetNumPendingEvents()  { return m_PendingEvents.size(); }
	private:
		static std::vector<EventListener *> m_EventListeners;
		static std::queue<Event *>          m_PendingEvents;
	};
}