#pragma once

#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#include <new>
#include <cstdlib>

#define LV_ENABLE_PROFILING 0
#define LV_MEM_PROFILING 0

#if !defined(LV_DIST) && LV_ENABLE_PROFILING

#define LV_MARK_FRAME FrameMark
#define LV_PROFILE_SCOPE(name) ZoneScopedN(name)

#if LV_MEM_PROFILING
void* operator new(size_t size);
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, size_t size) noexcept;
#endif

#else

#define LV_MARK_FRAME
#define LV_PROFILE_SCOPE

#endif
