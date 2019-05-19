#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API MouseEvent : public Event
	{
	public:
		virtual EventCategory GetCategory() const override { return Keyboard; }
	protected:
		MouseEvent()
		{}
	};

	class GOGAMAN_API MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(float positionX, float positionY)
			: MouseEvent(), m_PositionX(positionX), m_PositionY(positionY)
		{}

		static  EventType GetEventType() { return MouseMove; }
		virtual EventType GetType() const override { return GetEventType(); }
	protected:
		float m_PositionX;
		float m_PositionY;
	};

	class GOGAMAN_API MouseButtonPressEvent : public MouseEvent
	{
	public:
		MouseButtonPressEvent(const int keycode)
			: MouseEvent(), m_Keycode(keycode)
		{}

		static  EventType GetEventType() { return MouseButtonPress; }
		virtual EventType GetType() const override { return GetEventType(); }
	protected:
		int m_Keycode;
	};

	class GOGAMAN_API MouseButtonReleaseEvent : public MouseEvent
	{
	public:
		MouseButtonReleaseEvent(const int keycode)
			: MouseEvent(), m_Keycode(keycode)
		{}

		static  EventType GetEventType() { return MouseButtonRelease; }
		virtual EventType GetType() const override { return GetEventType(); }
	protected:
		int m_Keycode;
	};
}