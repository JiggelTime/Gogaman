#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API KeyboardEvent : public Event
	{
	public:
		virtual EventCategory GetCategory() const override { return Keyboard; }
	protected:
		KeyboardEvent(const int keycode)
			: m_Keycode(keycode)
		{}
	protected:
		int m_Keycode;
	};

	class GOGAMAN_API KeyPressEvent : public KeyboardEvent
	{
	public:
		KeyPressEvent(const int keycode, const int repeatCount)
			: KeyboardEvent(keycode), m_RepeatCount(repeatCount)
		{}

		static  EventType GetEventType() { return KeyPress; }
		virtual EventType GetType() const override { return GetEventType(); }
	private:
		int m_RepeatCount;
	};

	class GOGAMAN_API KeyRepeatEvent : public KeyboardEvent
	{
	public:
		KeyRepeatEvent(const int keycode, const int repeatCount)
			: KeyboardEvent(keycode), m_RepeatCount(repeatCount)
		{}

		static  EventType GetEventType() { return KeyRepeat; }
		virtual EventType GetType() const override { return GetEventType(); }
	private:
		int m_RepeatCount;
	};

	class GOGAMAN_API KeyReleaseEvent : public KeyboardEvent
	{
	public:
		KeyReleaseEvent(const int keycode)
			: KeyboardEvent(keycode)
		{}

		static EventType GetEventType() { return KeyRelease; }
		virtual EventType GetType() const override { return GetEventType(); }

	};
}