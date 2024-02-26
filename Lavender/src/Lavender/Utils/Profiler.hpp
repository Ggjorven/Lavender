#pragma once

#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#include <new>
#include <cstdlib>

#define LV_ENABLE_PROFILING 0

#if !defined(LV_DIST) && LV_ENABLE_PROFILING
#define LV_PROFILE_SCOPE(name) ZoneScopedN(name)

void* operator new(std::size_t size);
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, std::size_t size) noexcept;

#else
#define LV_PROFILE_SCOPE

#endif
