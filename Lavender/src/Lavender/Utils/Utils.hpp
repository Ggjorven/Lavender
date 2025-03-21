#pragma once

#include <stdint.h>
#include <memory>
#include <queue>
#include <type_traits>

#include "Lavender/Core/Logging.hpp"

#define BIT(x) (1 << x)

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
} \

#define LV_MAX_UINT8 255
#define LV_MAX_UINT16 65535 
#define LV_MAX_UINT32 4294967295 
#define LV_MAX_UINT64 18446744073709551615ULL 
#define LV_MAX_INT8 127 
#define LV_MAX_INT16 32767
#define LV_MAX_INT32 2147483647
#define LV_MAX_INT64 9223372036854775807LL
#define LV_MAX_FLOAT FLT_MAX 
#define LV_MAX_DOUBLE DBL_MAX

namespace Lavender::Utils
{

    // A class to be used for function queues
    template<typename Func>
    class Queue
    {
    public:
        Queue() = default;
        virtual ~Queue() = default;

        inline void Add(Func func) { m_Queue.push(func); }

        // Note(Jorben): Executing simultaneously clears the queue.
        template<typename ...Args>
        void Execute(Args&& ...args)
        {
            while (!m_Queue.empty())
            {
                Func& func = m_Queue.front();

                func(std::forward<Args>(args)...);

                m_Queue.pop();
            }
        }

        inline uint32_t Size() { return (uint32_t)m_Queue.size(); }

    private:
        std::queue<Func> m_Queue = { };
    };

}

// Note(Jorben): More like core functionality // TODO: Maybe move this?
namespace Lavender
{

    // Give std::shared_ptr a custom name
    template<typename T>
    using Ref = std::shared_ptr<T>;

    class RefHelper
    {
    public:
        template<typename T, typename... Args>
        static Ref<T> Create(Args&&... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        template<typename T, typename T2>
        static Ref<T> RefAs(Ref<T2> ptr)
        {
            Ref<T> newPtr = std::dynamic_pointer_cast<T>(ptr);

            if (!newPtr)
            {
                // TODO: Maybe make an assert?
                LV_LOG_ERROR("Failed to convert Ref<T2> to a Ref<T>");
            }

            return newPtr;
        }
    };

}