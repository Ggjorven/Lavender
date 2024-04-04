#include "lvpch.h"
#include "Profiler.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Utils/Utils.hpp"

#if !defined(LV_DIST) && LV_ENABLE_PROFILING
#if LV_MEM_PROFILING
void* operator new(std::size_t size) 
{
    if (Lavender::Application::Initialized())
        Lavender::Utils::ToolKit::GetAllocationCount()++;

    auto ptr = std::malloc(size);
    if (!ptr) 
        throw std::bad_alloc();

    TracyAlloc(ptr, size);
    return ptr;
}

void operator delete(void* ptr) noexcept 
{
    if (Lavender::Application::Initialized())
        Lavender::Utils::ToolKit::GetAllocationCount()--;

    TracyFree(ptr);
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t size) noexcept 
{
    if (Lavender::Application::Initialized())
        Lavender::Utils::ToolKit::GetAllocationCount()--;

    TracyFree(ptr);
    std::free(ptr);
}
#endif
#endif