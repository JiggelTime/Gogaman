#pragma once

#include "Gogaman/Base.h"
#include "Event.h"

namespace Gogaman
{
	class EventDispatcher
	{
	public:
		EventDispatcher(Event &event)
			: m_Event(event)
		{}

		template<typename T>
		bool Dispatch(std::function<bool(T &)> callback)
		{
			if(m_Event.GetType() == T::GetEventType())
			{
				m_Event.handled = callback(*(T *)&m_Event);
				return true;
			}

			return false;
		}
	private:
		Event &m_Event;
	};
}