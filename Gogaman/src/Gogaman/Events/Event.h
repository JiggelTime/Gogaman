#pragma once

#include "Gogaman/Core.h"
#include "pch.h"

namespace Gogaman
{
	enum EventType
	{
		//Keyboard
		KeyPress, KeyRelease, KeyRepeat
		//Mouse
	};

	enum EventCategory
	{
		Keyboard,
		Mouse
	};

	class GOGAMAN_API Event
	{
	public:
		virtual EventType     GetType()     const = 0;
		virtual EventCategory GetCategory() const = 0;
	public:
		bool handled;
	};

	class GOGAMAN_API EventDispatcher
	{
	public:
		EventDispatcher(Event &event)
			: m_Event(event)
		{}

		template<typename T>
		bool Dispatch(std::function<bool(T &)> &callback)
		{
			if(m_Event.handled)
				return false;

			if(m_Event.GetType() == T::GetEventType())
			{
				m_Event.handled = callback(m_Event);
				return true;
			}

			return false;
		}
	private:
		Event &m_Event;
	};
}