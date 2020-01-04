#pragma once

#include "Event.h"

namespace Gogaman
{
	class KeyboardEvent : public Event
	{
	public:
		inline virtual EventCategoryFlag GetCategory() const override { return EventCategoryFlag::Keyboard; }

		inline constexpr int GetKeycode() const { return m_Scancode; }
	protected:
		KeyboardEvent(const int scancode)
			: m_Scancode(scancode)
		{}
	protected:
		int m_Scancode;
	};

	class KeyPressEvent : public KeyboardEvent
	{
	public:
		KeyPressEvent(const int scancode, const int repeatCount)
			: KeyboardEvent(scancode), m_RepeatCount(repeatCount)
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::KeyPress; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline int GetRepeatCount() const { return m_RepeatCount; }
	private:
		int m_RepeatCount;
	};

	class KeyReleaseEvent : public KeyboardEvent
	{
	public:
		KeyReleaseEvent(const int scancode)
			: KeyboardEvent(scancode)
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::KeyRelease; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};
}