#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API EventDispatcher
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