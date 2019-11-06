#pragma once

#include "Event.h"

namespace Gogaman
{
	class EventListener;

	class EventManager
	{
	public:
		EventManager(const EventManager &)            = delete;
		EventManager &operator=(const EventManager &) = delete;

		inline void Send(std::unique_ptr<Event> &&event) { m_Events.emplace(std::move(event)); }

		void DispatchEvents();

		inline void AddListener(EventListener *listener) { m_Listeners.push_back(listener); }

		inline void Clear() { m_Events = {}; }

		inline size_t GetNumListeners()     const { return m_Listeners.size(); }
		inline size_t GetNumPendingEvents() const { return m_Events.size();  }

		static EventManager &GetInstance() { return *s_Instance; }
	private:
		EventManager();
		~EventManager() = default;
	private:
		std::vector<EventListener *>       m_Listeners;
		std::queue<std::unique_ptr<Event>> m_Events;

		static EventManager *s_Instance;
	};
}