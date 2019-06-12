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

		static inline  EventType GetTypeStatic() { return WindowClose; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(const int width, const int height)
			: m_Width(width), m_Height(height)
		{}

		static inline  EventType GetTypeStatic() { return WindowResize; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

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

		static inline  EventType GetTypeStatic() { return WindowFocus; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};

	class WindowUnfocusEvent : public WindowEvent
	{
	public:
		WindowUnfocusEvent()
		{}

		static inline  EventType GetTypeStatic() { return WindowUnfocus; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};
}