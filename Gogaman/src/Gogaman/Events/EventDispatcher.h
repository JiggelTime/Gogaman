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
		using EventCallback = std::function<bool(T &)>;

		template<typename T>
		bool Dispatch(EventCallback<T> callback)
		{
			if(m_Event.handled)
				return false;

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