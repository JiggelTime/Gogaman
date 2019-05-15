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

//#if GM_DEBUG
#if 1
	#define GM_ASSERT_ENABLE 1
#else
	#define GM_ASSERT_ENABLE 0
#endif

#if GM_ASSERT_ENABLE
	#define GM_ASSERT(x, ...) \
	if(!(x)) \
	{ \
		GM_LOG_CORE_ERROR("Assertion failed at %s: Line: %d Condition: %s", __FILE__, __LINE__, #x); \
		abort(); \
	}
#else
	#define GM_ASSERT(x, ...)
#endif

#define GM_BIND_EVENT_CALLBACK(x) std::bind(&x, this, std::placeholders::_1)