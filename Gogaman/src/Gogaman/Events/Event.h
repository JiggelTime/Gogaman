#pragma once

namespace Gogaman
{
	enum class EventType : uint8_t
	{
		//Window
		WindowClose, WindowResize, WindowFocus, WindowUnfocus, WindowFileDrop,
		//Keyboard
		KeyPress, KeyRelease,
		//Mouse
		MouseMove, MouseScroll, MouseButtonPress, MouseButtonRelease
	};

	enum class EventCategoryFlag : uint8_t
	{
		None     = 0,
		Window   = 1,
		Input    = 1 << 1,
		Keyboard = 1 << 2,
		Mouse    = 1 << 3
	};

	class Event
	{
	public:
		virtual EventType         GetType()     const = 0;
		virtual EventCategoryFlag GetCategory() const = 0;
	public:
		bool isHandled;
	};
}