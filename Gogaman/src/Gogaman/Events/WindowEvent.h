#pragma once

#include "Event.h"

namespace Gogaman
{
	class WindowEvent : public Event
	{
	public:
		inline virtual EventCategoryFlag GetCategory() const override { return EventCategoryFlag::Window; }
	protected:
		WindowEvent()
		{}
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent()
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::WindowClose; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(const uint16_t width, const uint16_t height)
			: m_Width(width), m_Height(height)
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::WindowResize; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr uint16_t GetWidth()  const { return m_Width;  }
		inline constexpr uint16_t GetHeight() const { return m_Height; }
	private:
		uint16_t m_Width, m_Height;
	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		WindowFocusEvent()
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::WindowFocus; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};

	class WindowUnfocusEvent : public WindowEvent
	{
	public:
		WindowUnfocusEvent()
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::WindowUnfocus; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }
	};

	class WindowFileDropEvent : public WindowEvent
	{
	public:
		WindowFileDropEvent(std::vector<std::string> &&filepaths)
			: m_Filepaths(std::move(filepaths))
		{}

		static inline constexpr EventType GetTypeStatic() { return EventType::WindowFileDrop; }
		inline virtual EventType GetType() const override { return GetTypeStatic(); }

		inline constexpr const std::vector<std::string> &GetFilepaths() const { return m_Filepaths; }
	private:
		std::vector<std::string> m_Filepaths;
	};
}