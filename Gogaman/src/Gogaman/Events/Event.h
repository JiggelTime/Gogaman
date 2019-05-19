#pragma once

#include "Gogaman/Core.h"

namespace Gogaman
{
	enum EventType
	{
		//Keyboard
		KeyPress, KeyRelease, KeyRepeat,
		//Mouse
		MouseMove, MouseButtonPress, MouseButtonRelease
	};

	enum EventCategory
	{
		Keyboard,
		Mouse
	};

	class GOGAMAN_API Event
	{
	public:
		virtual EventType     GetType()     const = 0;
		virtual EventCategory GetCategory() const = 0;
	public:
		bool handled;
	};
}