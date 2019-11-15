#pragma once

#include "Event.h"

namespace Gogaman
{
	class EventDispatcher
	{
	public:
		EventDispatcher(Event &event)
			: m_Event(event)
		{}

		template<typename EventType, typename CallbackFunction>
		inline bool Dispatch(const CallbackFunction &callbackFunction)
		{
			if(m_Event.GetType() == EventType::GetTypeStatic())
			{
				m_Event.isHandled = callbackFunction(static_cast<EventType &>(m_Event));
				return true;
			}

			return false;
		}
	private:
		Event &m_Event;
	};
}