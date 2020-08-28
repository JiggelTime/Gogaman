#pragma once

#include "Logging/Log.h"

//Debug vs release build features
#if 1
	#define GM_DEBUG_ASSERTIONS_ENABLED           TRUE
	#define GM_STABLE_ARRAY_ID_VALIDATION_ENABLED TRUE
	#define GM_RHI_DEBUGGING_ENABLED              TRUE
#else
	#define GM_ASSERTIONS_ENABLED                 FALSE
	#define GM_STABLE_ARRAY_ID_VALIDATION_ENABLED FALSE
	#define GM_RHI_DEBUGGING_ENABLED              FALSE
#endif

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

#if GM_DEBUG_ASSERTIONS_ENABLED
	#define GM_DEBUG_ASSERT(x, ...)            GM_ASSERT(x, __VA_ARGS__)
	#define GM_DEBUG_STATIC_ASSERT(x, message) GM_STATIC_ASSERT(x, message)
#else
	#define GM_DEBUG_ASSERT(x, ...)
	#define GM_DEBUG_STATIC_ASSERT(x, message)
#endif

#define GM_FORCE_INLINE __forceinline

/*
void *AllocateVirtual(const uint64_t size);
//void CommitVirtual(void *memory, const uint64_t size);
void DeallocateVirtual(void *memory, const uint64_t size);

#ifdef GM_PLATFORM_WINDOWS
	void *AllocateVirtual(const uint64_t size)
	{
		void *memory = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
		GM_ASSERT(memory, "Failed to allocate virtual memory");
		return memory;
	}

	void CommitVirtual(void *memory, const uint64_t size)
	{
		//VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
	}

	void DeallocateVirtual(void *memory, const uint64_t size)
	{
		GM_ASSERT(VirtualFree(memory, 0, MEM_RELEASE), "Failed to deallocate virtual memory");
	}
#elif defined GM_PLATFORM_LINUX | GM_PLATFORM_OSX
	void *AllocateVirtual(const uint64_t size)
	{
		void *memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE | MAP_ANONYMOUS, -1, 0);
		GM_ASSERT(memory, "Failed to allocate virtual memory");
		return memory;
	}

	void CommitVirtual(void *memory, const uint64_t size)
	{}

	void DeallocateVirtual(void *memory, const uint64_t size)
	{
		GM_ASSERT(!munmap(memory, size), "Failed to deallocate virtual memory");
	}
#else
	#error "Unsupported platform"
#endif
*/

#define GM_BIND_EVENT_CALLBACK(x) std::bind(&x, this, std::placeholders::_1)

//Renderer
#define GM_RENDERING_API_NONE   0
#define GM_RENDERING_API_VULKAN 1

#define GM_RENDERING_API GM_RENDERING_API_VULKAN