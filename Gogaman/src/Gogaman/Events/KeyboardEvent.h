#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API KeyboardEvent : public Event
	{
	public:
		inline virtual EventCategory GetCategory() const override { return EventCategory::Keyboard; }

		inline int GetKeycode() const { return m_Scancode; }
	protected:
		KeyboardEvent(const int scancode)
			: m_Scancode(scancode)
		{}
	protected:
		int m_Scancode;
	};

	class GOGAMAN_API KeyPressEvent : public KeyboardEvent
	{
	public:
		KeyPressEvent(const int scancode, const int repeatCount)
			: KeyboardEvent(scancode), m_RepeatCount(repeatCount)
		{}

		inline static  EventType GetEventType() { return KeyPress; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline int GetRepeatCount() const { return m_RepeatCount; }
	private:
		int m_RepeatCount;
	};

	class GOGAMAN_API KeyReleaseEvent : public KeyboardEvent
	{
	public:
		KeyReleaseEvent(const int scancode)
			: KeyboardEvent(scancode)
		{}

		inline static  EventType GetEventType() { return KeyRelease; }
		inline virtual EventType GetType() const override { return GetEventType(); }
	};
}