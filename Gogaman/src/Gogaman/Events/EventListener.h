#pragma once

#include "Gogaman/Core.h"
#include "Event.h"

namespace Gogaman
{
	class GOGAMAN_API EventListener
	{
	public:
		virtual bool OnEvent(Event &event) = 0;
	};
}