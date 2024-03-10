#pragma once

#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#include <new>
#include <cstdlib>

#define LV_ENABLE_PROFILING 1

#if !defined(LV_DIST) && LV_ENABLE_PROFILING
#define LV_MARK_FRAME FrameMark

#define LV_PROFILE_SCOPE(name) ZoneScopedN(name)

void* operator new(std::size_t size);
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, std::size_t size) noexcept;

#else
#define LV_MARK_FRAME

#define LV_PROFILE_SCOPE

#endif
