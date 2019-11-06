#pragma once

#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsTime.h"

	namespace Gogaman { using Time = WindowsTime; }
#else
	#error
#endif