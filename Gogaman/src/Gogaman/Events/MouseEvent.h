#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API MouseEvent : public Event
	{
	public:
		inline virtual EventCategory GetCategory() const override { return EventCategory::Mouse; }
	protected:
		MouseEvent()
		{}
	};

	class GOGAMAN_API MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(float positionX, float positionY)
			: m_PositionX(positionX), m_PositionY(positionY)
		{}

		inline static  EventType GetEventType() { return MouseMove; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline float GetPositionX() const { return m_PositionX; }
		inline float GetPositionY() const { return m_PositionY; }
	private:
		float m_PositionX;
		float m_PositionY;
	};

	class GOGAMAN_API MouseScrollEvent : public MouseEvent
	{
	public:
		MouseScrollEvent(float offsetX, float offsetY)
			: m_OffsetX(offsetX), m_OffsetY(offsetY)
		{}

		inline static  EventType GetEventType() { return MouseScroll; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline float GetOffsetX() const { return m_OffsetX; }
		inline float GetOffsetY() const { return m_OffsetY; }
	private:
		float m_OffsetX;
		float m_OffsetY;
	};

	class GOGAMAN_API MouseButtonPressEvent : public MouseEvent
	{
	public:
		MouseButtonPressEvent(const int scancode)
			: m_Scancode(scancode)
		{}

		inline static  EventType GetEventType() { return MouseButtonPress; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};

	class GOGAMAN_API MouseButtonReleaseEvent : public MouseEvent
	{
	public:
		MouseButtonReleaseEvent(const int scancode)
			: m_Scancode(scancode)
		{}

		inline static  EventType GetEventType() { return MouseButtonRelease; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};
}