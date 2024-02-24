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

namespace Utils
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

    // Note(Jorben): This function name is not the most descriptive since I didn't want it to be a long name,
    // but all this function does is convert a shared_ptr of one type to another type.
    template<typename T, typename T2>
    std::shared_ptr<T> As(std::shared_ptr<T2> ptr)
    {
        std::shared_ptr<T> newPtr = std::dynamic_pointer_cast<T>(ptr);

        if (!newPtr)
        {
            // TODO: Maybe make an assert?
            LV_LOG_ERROR("Failed to convert std::shared_ptr<T2> to a std::shared_ptr<T>");
        }

        return newPtr;
    }

}