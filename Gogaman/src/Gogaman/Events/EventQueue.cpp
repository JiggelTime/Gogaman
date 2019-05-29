#include "pch.h"
#include "EventQueue.h"

#include "Gogaman/Logging/Log.h"
#include "Event.h"
#include "EventListener.h"

namespace Gogaman
{
	EventQueue *EventQueue::s_Instance = new EventQueue();

	EventQueue::EventQueue()
	{
		GM_ASSERT(s_Instance == nullptr, "Failed to construct event queue: instance already exists");
	}

	EventQueue::~EventQueue()
	{}
	
	void EventQueue::DispatchEvents()
	{
		while(!m_PendingEvents.empty())
		{
			for(auto listener : m_Listeners)
			{
				listener->OnEvent(*m_PendingEvents.front().get());
			}

			m_PendingEvents.pop();
		}
	}
}