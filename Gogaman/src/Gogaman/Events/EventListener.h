#pragma once

#include "Gogaman/Base.h"
#include "Event.h"
#include "EventManager.h"

namespace Gogaman
{
	class EventListener
	{
	public:
		virtual void OnEvent(Event &event) = 0;
	protected:
		EventListener()
		{
			EventManager::GetInstance().AddListener(this);
		}
	};
}