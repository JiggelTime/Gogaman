#pragma once

#define GM_RENDERING_API_OPENGL 1
#define GM_RENDERING_API_VULKAN 2

#define GM_RENDERING_API GM_RENDERING_API_OPENGL

#if 1
	#define GM_ASSERTIONS_ENABLED 1
#else
	#define GM_ASSERTIONS_ENABLED 0
#endif

#if GM_ASSERTIONS_ENABLED
	#define GM_ASSERT(x, ...)\
	if(!(x))\
	{\
		GM_LOG_CORE_ERROR("   _______ _________   __ ");\
		GM_LOG_CORE_ERROR("  / __/ _ /_  __/ _ | / / ");\
		GM_LOG_CORE_ERROR(" / _// __ |/ / / __ |/ /__");\
		GM_LOG_CORE_ERROR("/_/ /_/ |_/_/ /_/ |_/____/");\
		GM_LOG_CORE_ERROR("");\
		GM_LOG_CORE_ERROR("Assertion failed at %s | Line: %d | Condition: %s", __FILE__, __LINE__, #x);\
		GM_LOG_CORE_ERROR(__VA_ARGS__);\
		abort();\
	}

	#define GM_STATIC_ASSERT(x, message) static_assert(x, message);
#else
	#define GM_ASSERT(x, ...) ;
	#define GM_STATIC_ASSERT(x, message)
#endif

#if 1
	#define GM_DEBUG_SHADER_UNIFORMS_ENABLED 1
#else
	#define GM_DEBUG_SHADER_UNIFORMS_ENABLED 0
#endif

#define GM_FORCE_INLINE __forceinline

#define GM_BIND_EVENT_CALLBACK(x) std::bind(&x, this, std::placeholders::_1)