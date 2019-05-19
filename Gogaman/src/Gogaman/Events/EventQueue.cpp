#include "pch.h"
#include "EventQueue.h"
#include "EventListener.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	std::vector<EventListener *> EventQueue::m_EventListeners;
	std::queue<Event *>          EventQueue::m_PendingEvents;

	void EventQueue::Enqueue(Event &event)
	{
		m_PendingEvents.push(std::move(&event));
	}
	
	void EventQueue::DispatchEvents()
	{
		while(!m_PendingEvents.empty())
		{
			for(auto listener : m_EventListeners)
			{
				listener->OnEvent(*m_PendingEvents.front());
			}

			m_PendingEvents.pop();
		}
	}

	void EventQueue::AddListener(EventListener &listener)
	{
		m_EventListeners.push_back(&listener);
	}
	
	void EventQueue::RemoveListenerFromEvent(EventListener &listener, Event &event)
	{

	}

	void EventQueue::RemoveListener(EventListener &listener)
	{

	}
}