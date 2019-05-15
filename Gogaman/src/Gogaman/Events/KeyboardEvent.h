#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

#include <iostream>

namespace Gogaman
{
	class GOGAMAN_API KeyboardEvent : Event
	{
		virtual EventCategory GetCategory() const { return Keyboard; }
	protected:
		KeyboardEvent(int keycode)
			: m_Keycode(keycode)
		{}
	protected:
		int m_Keycode;
	};

	class GOGAMAN_API KeyPressEvent : KeyboardEvent
	{
	public:
		KeyPressEvent(int keycode, int repeatCount)
			: KeyboardEvent(keycode), m_RepeatCount(repeatCount)
		{}

		static  EventType GetEventType() { return KeyPress; }
		virtual EventType GetType() const { return GetEventType(); }
	private:
		int m_RepeatCount;
	};
}