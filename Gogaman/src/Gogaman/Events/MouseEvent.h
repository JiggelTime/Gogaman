#pragma once

#include "Gogaman/Base.h"
#include "Event.h"

namespace Gogaman
{
	class MouseEvent : public Event
	{
	public:
		inline virtual EventCategory GetCategory() const override { return EventCategory::Mouse; }
	protected:
		MouseEvent()
		{}
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(float positionX, float positionY)
			: m_PositionX(positionX), m_PositionY(positionY)
		{}

		static constexpr EventType GetTypeStatic() { return MouseMove; }
		inline virtual   EventType GetType() const override { return GetTypeStatic(); }

		inline float GetPositionX() const { return m_PositionX; }
		inline float GetPositionY() const { return m_PositionY; }
	private:
		float m_PositionX;
		float m_PositionY;
	};

	class MouseScrollEvent : public MouseEvent
	{
	public:
		MouseScrollEvent(float offsetX, float offsetY)
			: m_OffsetX(offsetX), m_OffsetY(offsetY)
		{}

		static constexpr EventType GetTypeStatic() { return MouseScroll; }
		inline virtual   EventType GetType() const override { return GetTypeStatic(); }

		inline float GetOffsetX() const { return m_OffsetX; }
		inline float GetOffsetY() const { return m_OffsetY; }
	private:
		float m_OffsetX;
		float m_OffsetY;
	};

	class MouseButtonPressEvent : public MouseEvent
	{
	public:
		MouseButtonPressEvent(const int scancode)
			: m_Scancode(scancode)
		{}

		static constexpr EventType GetTypeStatic() { return MouseButtonPress; }
		inline virtual   EventType GetType() const override { return GetTypeStatic(); }

		inline int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};

	class MouseButtonReleaseEvent : public MouseEvent
	{
	public:
		MouseButtonReleaseEvent(const int scancode)
			: m_Scancode(scancode)
		{}

		static constexpr EventType GetTypeStatic() { return MouseButtonRelease; }
		inline virtual   EventType GetType() const override { return GetTypeStatic(); }

		inline int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};
}