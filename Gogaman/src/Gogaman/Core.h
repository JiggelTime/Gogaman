#pragma once

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
	#define GM_LOG(x) std::cout << x << std::endl;
#else
	#define GM_LOG(x)
#endif

#if GM_DEBUG
	#define GM_ASSERT(x, ...) \
	if(!x) \
	{ \
		std::cout << "ERROR: Assertion failed" << std::endl; \
		std::cout << __FILE__ << ": Line " << __LINE__ << std::endl; \
		std::cout << "Condition: " << #x << std::endl; \
		__debugbreak(); \
	}
#else
	#define GM_ASSERT(x, ...)
#endif