#include "pch.h"
#include "EventQueue.h"

namespace Gogaman
{
	EventQueue::EventQueue()
	{
		m_PendingEvents.reserve(m_MaxPendingEvents);
	}

	EventQueue::~EventQueue()
	{

	}

	void EventQueue::AddListener(EventListener &listener, Event &event)
	{

	}
	
	void EventQueue::RemoveListenerEvent(EventListener &listener, Event &event)
	{

	}

	void EventQueue::RemoveListener(EventListener &listener)
	{

	}
}