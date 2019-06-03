#pragma once

#include "Gogaman/Base.h"
#include "EventQueue.h"

namespace Gogaman
{
	enum EventType
	{
		//Window
		WindowClose, WindowResize, WindowFocus, WindowUnfocus,
		//Keyboard
		KeyPress, KeyRelease,
		//Mouse
		MouseMove, MouseScroll, MouseButtonPress, MouseButtonRelease
	};

	enum class EventCategory
	{
		Window,
		Input,
		Keyboard,
		Mouse
	};

	class Event
	{
	public:
		virtual EventType     GetType()     const = 0;
		virtual EventCategory GetCategory() const = 0;
	public:
		bool handled;
	};
}