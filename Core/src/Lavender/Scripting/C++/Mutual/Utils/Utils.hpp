#pragma once

#include <stdint.h>

#include <map>
#include <mutex>
#include <queue>
#include <future>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <filesystem>
#include <type_traits>
#include <unordered_map>

#define BIT(x) (1 << x)
#define BIT_X(x, y) (x << y)

#define DEFINE_BITWISE_OPS(Type) \
constexpr Type operator | (Type lhs, Type rhs) \
{ \
    return static_cast<Type>(static_cast<int>(lhs) | static_cast<int>(rhs)); \
} \
constexpr bool operator & (Type lhs, Type rhs) \
{ \
    return static_cast<int>(lhs) & static_cast<int>(rhs); \
} \
constexpr Type operator ^ (Type lhs, Type rhs) \
{ \
    return static_cast<Type>(static_cast<int>(lhs) ^ static_cast<int>(rhs)); \
}

namespace Lavender::Utils
{

    class FastRandom
    {
    public:
        inline static void Init() // Gets called by Tools::Init
        {
            std::srand(static_cast<unsigned>(std::time(NULL)));
        }

    public:
        inline static int32_t Int(int32_t min = 0, int32_t max = 2147483647)
        {
            return (min + std::rand() % (max - min + 1));
        }

        inline static const char* CStr()
        {
            static std::string str = {};
            str = std::to_string(Int());

            return str.c_str();
        }
    };

    class Tools 
    {
    public:
        inline static void Init() // Gets called when script is loaded
        {
            FastRandom::Init();
        }
    };

#if defined(LAVENDER_DLL)
    extern "C" inline __declspec(dllexport) void Lavender_InitTools()
    {
        Tools::Init();
    }
#endif

}