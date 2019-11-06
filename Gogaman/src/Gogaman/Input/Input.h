#pragma once

#include "Gogaman/Core/Base.h"
#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"

	namespace Gogaman { using Input = WindowsInput; }
#else
	#error
#endif