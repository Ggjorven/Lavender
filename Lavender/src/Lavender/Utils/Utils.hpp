#pragma once

#include <stdint.h>

#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <type_traits>
#include <unordered_map>

#include "Lavender/Core/Logging.hpp"

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

    // Platform specific
    class ToolKit
    {
    public:
        static float GetTime() { return s_Instance->GetTimeImpl(); }

    private:
        virtual float GetTimeImpl() const = 0;

    private:
        static std::unique_ptr<ToolKit> s_Instance;
    };

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

    class Timer
    {
    public:
        Timer()
            : m_Start(ToolKit::GetTime())
        {
        }

        inline float GetPassedTime() const { return (ToolKit::GetTime() - m_Start); }

    private:
        float m_Start = 0.0f;
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

    template<typename Key, typename Value>
    using Dict = std::unordered_map<Key, Value>;

    template<typename Key, typename Value>
    class SortedDict
    {
    public:
        SortedDict() = default;
        SortedDict(const SortedDict<Key, Value>& other) = default;
        virtual ~SortedDict() = default;

        Value& operator [] (const Key& key)
        {
            auto it = m_Indices.find(key);
            if (it == m_Indices.end())
            {
                m_Items.push_back(std::make_pair(key, Value()));
                m_Indices[key] = m_Items.size() - 1;
            }
            return m_Items[m_Indices[key]].second;
        }

        void insert(const Key& key, const Value& value)
        {
            if (m_Indices.find(key) == m_Indices.end())
            {
                m_Items.push_back(std::make_pair(key, value));
                m_Indices[key] = m_Items.size() - 1;
            }
            else
            {
                m_Items[m_Indices[key]].second = value;
            }
        }

        void erase(const Key& key)
        {
            if (m_Indices.find(key) != m_Indices.end())
            {
                m_Items.erase(m_Items.begin() + m_Indices[key]);
                m_Indices.erase(key);
            }
        }

        auto begin() { return m_Items.begin(); }
        const auto begin() const { return m_Items.begin(); }
        auto end() { return m_Items.end(); }
        const auto end() const { return m_Items.end(); }

    public:
        std::unordered_map<Key, size_t> m_Indices = { };
        std::vector<std::pair<Key, Value>> m_Items = { };
    };

}