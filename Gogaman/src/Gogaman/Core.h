#pragma once

#include "TypeDefs.h"

#ifdef GM_PLATFORM_WINDOWS
	#ifdef GM_BUILD_DLL
		#define GOGAMAN_API __declspec(dllexport)
	#else
		#define GOGAMAN_API __declspec(dllimport)
	#endif
#else
	#error
#endif

#if GM_DEBUG
	#define GM_ASSERT(x, ...) \
	if(!(x)) \
	{ \
		std::cerr << "Assertion failed at " << __FILE__ << ": line " << __LINE__ << std::endl; \
		std::cerr << "Condition: " << #x << std::endl; \
		abort(); \
	}
#else
	#define GM_ASSERT(x, ...)
#endif