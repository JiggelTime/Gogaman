#pragma once

#if 1
	#define GM_ASSERTIONS_ENABLED 1
#else
	#define GM_ASSERTIONS_ENABLED 0
#endif

#if GM_ASSERTIONS_ENABLED
	#define GM_ASSERT(x, ...)\
	if(!(x))\
	{ \
		GM_LOG_CORE_ERROR("Assertion failed at %s | Line: %d | Condition: %s | Reason: %s", __FILE__, __LINE__, #x, __VA_ARGS__);\
		abort();\
	}
#else
	#define GM_ASSERT(x, ...)
#endif

#define GM_BIND_EVENT_CALLBACK(x) std::bind(&x, this, std::placeholders::_1)