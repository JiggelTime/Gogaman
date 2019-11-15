#pragma once

#include "Gogaman/Core/Base.h"
#ifdef GM_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"

	namespace Gogaman
	{
		using Window = WindowsWindow;
	}
#else
	#error "Unsupported platform"
#endif