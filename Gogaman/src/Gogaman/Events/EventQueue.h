#pragma once

#include "Gogaman/Base.h"

namespace Gogaman
{
	class Event;
	class EventListener;

	class EventQueue
	{
	public:
		EventQueue(const EventQueue &)            = delete;
		EventQueue &operator=(const EventQueue &) = delete;

		inline void Enqueue(std::unique_ptr<Event> &&event) { m_PendingEvents.push(std::move(event)); }

		void DispatchEvents();

		inline void AddListener(EventListener *listener) { m_Listeners.push_back(listener); }

		inline void Clear() { m_PendingEvents = {}; }

		inline size_t GetNumListeners()     const { return m_Listeners.size(); }
		inline size_t GetNumPendingEvents() const { return m_PendingEvents.size();  }

		static EventQueue &GetInstance() { return *s_Instance; }
	private:
		EventQueue();
		~EventQueue();
	private:
		std::vector<EventListener *>       m_Listeners;
		std::queue<std::unique_ptr<Event>> m_PendingEvents;

		static EventQueue *s_Instance;
	};
}