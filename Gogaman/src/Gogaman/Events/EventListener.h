#pragma once

#include "Gogaman/Base.h"
#include "Event.h"
#include "EventQueue.h"

namespace Gogaman
{
	class EventListener
	{
	public:
		virtual void OnEvent(Event &event) = 0;
	protected:
		EventListener()
		{
			EventQueue::GetInstance().AddListener(this);
		}
	};
}