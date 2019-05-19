#pragma once

#include "Gogaman/Core.h"
#include "Event.h"
#include "EventQueue.h"

namespace Gogaman
{
	class GOGAMAN_API EventListener
	{
	public:
		EventListener();

		virtual void OnEvent(Event &event) = 0;
	};
}