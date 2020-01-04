#pragma once

#include "Event.h"

namespace Gogaman
{
	class MouseEvent : public Event
	{
	public:
		inline virtual EventCategoryFlag GetCategory() const override { return EventCategoryFlag::Mouse; }
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

		static inline constexpr EventType GetTypeStatic() { return EventType::MouseMove; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr float GetPositionX() const { return m_PositionX; }
		inline constexpr float GetPositionY() const { return m_PositionY; }
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

		static inline constexpr EventType GetTypeStatic() { return EventType::MouseScroll; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr float GetOffsetX() const { return m_OffsetX; }
		inline constexpr float GetOffsetY() const { return m_OffsetY; }
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

		static inline constexpr EventType GetTypeStatic() { return EventType::MouseButtonPress; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};

	class MouseButtonReleaseEvent : public MouseEvent
	{
	public:
		MouseButtonReleaseEvent(const int scancode)
			: m_Scancode(scancode)
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::MouseButtonRelease; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr int GetButton() const { return m_Scancode; }
	private:
		int m_Scancode;
	};
}