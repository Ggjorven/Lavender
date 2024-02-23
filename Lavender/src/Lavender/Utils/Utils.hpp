#pragma once

#include <stdint.h>
#include <queue>
#include <type_traits>

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

}