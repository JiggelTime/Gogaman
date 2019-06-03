#pragma once

#include "Gogaman/Base.h"
#include "Event.h"

namespace Gogaman
{
	class WindowEvent : public Event
	{
	public:
		inline virtual EventCategory GetCategory() const override { return EventCategory::Window; }
	protected:
		WindowEvent()
		{}
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent()
		{}

		inline static  EventType GetEventType() { return WindowClose; }
		inline virtual EventType GetType() const override { return GetEventType(); }
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(const int width, const int height)
			: m_Width(width), m_Height(height)
		{}

		inline static  EventType GetEventType() { return WindowResize; }
		inline virtual EventType GetType() const override { return GetEventType(); }

		inline int GetWidth()  const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
	private:
		int m_Width, m_Height;
	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		WindowFocusEvent()
		{}

		inline static  EventType GetEventType() { return WindowFocus; }
		inline virtual EventType GetType() const override { return GetEventType(); }
	};

	class WindowUnfocusEvent : public WindowEvent
	{
	public:
		WindowUnfocusEvent()
		{}

		inline static  EventType GetEventType() { return WindowUnfocus; }
		inline virtual EventType GetType() const override { return GetEventType(); }
	};
}