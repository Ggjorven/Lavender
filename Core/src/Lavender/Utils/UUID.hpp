#pragma once

#include <string>
#include <random>
#include <cstdint>
#include <type_traits>

namespace Lavender
{

    class UUID
    {
    public:
        UUID()
        {
            std::uniform_int_distribution<uint64_t> distribution(0ULL, std::numeric_limits<uint64_t>::max());
            m_UUID = distribution(s_RandomGenerator);
        }
        UUID(uint64_t uuid)
            : m_UUID(uuid)
        {
        }
        virtual ~UUID() = default;

        inline std::string String() const
        {
            return std::to_string((uint64_t)m_UUID);
        }

        inline operator uint64_t() const 
        { 
            return m_UUID; 
        }

        inline operator std::string() const
        {
            return String();
        }

        inline bool operator == (const UUID& other) const
        {
            return m_UUID == other.m_UUID;
        }

        inline bool operator < (const UUID& rhs) const
        {
            return m_UUID < rhs.m_UUID;
        }

    public:
        static const UUID Empty;
    
    private:
        inline static std::random_device s_RandomDevice = {};
        inline static std::mt19937_64 s_RandomGenerator = std::mt19937_64(s_RandomDevice());

        uint64_t m_UUID = 0ull;
    };
    inline const UUID UUID::Empty = UUID(0);

}

namespace std 
{
    template<>
    struct hash<Lavender::UUID> 
    {
        // Define the operator() to generate the hash
        uint64_t operator()(const Lavender::UUID& uuid) const 
        {
            return (uint64_t)uuid;
        }
    };
}