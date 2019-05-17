#include "pch.h"
#include "EventQueue.h"

namespace Gogaman
{
	EventQueue::EventQueue()
	{}

	EventQueue::~EventQueue()
	{}

	void EventQueue::ProcessEvents()
	{
		for(auto &i : m_EventListeners)
		{
			i.OnEvent();
		}
	}

	void EventQueue::AddListener(EventListener &listener, Event &event)
	{
		m_EventListeners.push_back(listener);
		m_PendingEvents.push_back(event);
	}
	
	void EventQueue::RemoveListenerEvent(EventListener &listener, Event &event)
	{

	}

	void EventQueue::RemoveListener(EventListener &listener)
	{

	}
}