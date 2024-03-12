#pragma once

#include <cstdint>
#include <type_traits>

namespace Lavender 
{

    // Note(Jorben): This is not a fully functional UINT128
    struct UINT128 final 
    {
    public:
        uint64_t Low = 0ull;
        uint64_t High = 0ull;

    public:
        constexpr UINT128() = default;

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128(T&& num) noexcept
            : Low(static_cast<std::uint64_t>(num)), High(static_cast<std::uint64_t>((num >= 0) ? 0 : -1))
        {
        }

        template<typename T>
        friend constexpr UINT128 operator&(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs &= rhs;
        }

        constexpr UINT128& operator&=(const UINT128& rhs) noexcept 
        {
            Low &= rhs.Low;
            High &= rhs.High;

            return *this;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator&=(T&& rhs) noexcept 
        {
            Low &= rhs;
            High = 0;

            return *this;
        }

        template<typename T>
        friend constexpr UINT128 operator|(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs |= rhs;
        }

        constexpr UINT128& operator|=(const UINT128& rhs) noexcept 
        {
            Low |= rhs.Low;
            High |= rhs.High;

            return *this;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator|=(T&& rhs) noexcept 
        {
            Low |= rhs;

            return *this;
        }

        template<typename T>
        friend constexpr UINT128 operator^(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs ^= rhs;
        }

        constexpr UINT128& operator^=(const UINT128& rhs) noexcept 
        {
            Low ^= rhs.Low;
            High ^= rhs.High;

            return *this;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator^=(T&& rhs) noexcept 
        {
            Low ^= rhs;

            return *this;
        }

        friend constexpr UINT128 operator~(UINT128 value) noexcept 
        {
            value.Low = ~value.Low;
            value.High = ~value.High;

            return value;
        }

        template<typename T>
        friend constexpr UINT128 operator<<(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs <<= rhs;
        }

        constexpr UINT128& operator<<=(const UINT128& rhs) noexcept 
        {
            if (rhs.High > 0) 
            {
                High = 0ull;
                Low = 0ull;
                return *this;
            }

            return *this <<= rhs.Low;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator<<=(T&& rhs) noexcept
        {
            if (rhs < 64) 
            {
                if (rhs != 0) 
                {
                    High = (High << rhs) | (Low >> (64 - rhs));
                    Low = Low << rhs;
                }
            }
            else if (rhs < 128) 
            {
                Low = 0;
                High = High << (rhs - 64);
            }
            else 
            {
                High = 0ull;
                Low = 0ull;
            }

            return *this;
        }

        template<typename T>
        friend constexpr UINT128 operator>>(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs >>= rhs;
        }

        constexpr UINT128& operator>>=(const UINT128& rhs) noexcept 
        {
            if (rhs.High > 0) 
            {
                High = 0;
                Low = 0;
                return *this;
            }

            return *this >>= rhs.Low;
        }


        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator>>=(T&& rhs) noexcept
        {
            if (rhs == 0)
                return *this;

            const auto backup = Low;
            Low = High;
            if (rhs < 64) 
            {
                High >>= rhs;
                Low <<= 64 - rhs;
                Low |= backup >> rhs;
            }
            else 
            {
                High = 0;
                Low >>= rhs - 64;
            }

            return *this;
        }



        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr bool operator==(T&& rhs) const noexcept 
        {
            return Low == static_cast<std::uint64_t>(rhs) && High == 0;
        }

        constexpr bool operator==(const UINT128& rhs) const noexcept 
        {
            return Low == rhs.Low && High == rhs.High;
        }

        template<typename T>
        constexpr bool operator!=(T&& rhs) const noexcept 
        {
            return !(*this == rhs);
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr bool operator<(T&& rhs) const noexcept 
        {
            return Low < static_cast<std::uint64_t>(rhs) && High == 0;
        }

        constexpr bool operator<(const UINT128& rhs) const noexcept 
        {
            if (High == rhs.High)
                return Low < rhs.Low;

            return High < rhs.High;
        }

        template<typename T>
        constexpr bool operator>(T&& rhs) const noexcept 
        {
            return *this >= rhs && *this != rhs;
        }

        template<typename T>
        constexpr bool operator<=(T&& rhs) const noexcept 
        {
            return !(*this > rhs);
        }

        template<typename T>
        constexpr bool operator>=(T&& rhs) const noexcept {

            return !(*this < rhs);
        }

        template<typename T>
        friend constexpr UINT128 operator+(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs += rhs;
        }

        constexpr UINT128& operator+=(const UINT128& rhs) noexcept 
        {
            Low += rhs.Low;
            High += rhs.High;

            return *this;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator+=(T&& rhs) noexcept 
        {
            // Note(Jorben): This is an overflow guard.
            if (Low + rhs < Low)
                ++High;
            Low += rhs;

            return *this;
        }

        template<typename T>
        friend constexpr UINT128 operator-(UINT128 lhs, T&& rhs) noexcept 
        {
            return lhs -= rhs;
        }

        constexpr UINT128& operator-=(const UINT128& rhs) noexcept 
        {
            Low -= rhs.Low;
            High -= rhs.High;

            return *this;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<std::decay_t<T>>>>
        constexpr UINT128& operator-=(T&& rhs) noexcept 
        {
            // Note(Jorben): This is an overflow guard.
            if (Low - rhs > Low)
                --High;
            Low -= rhs;
            return *this;
        }
    };

}

typedef Lavender::UINT128 uint128_t;

namespace Lavender
{

    // 64 bit
    class UUID
    {
    public:
        UUID() = default;
        UUID(uint64_t uuid);
        virtual ~UUID() = default;

        static UUID Create();

        inline uint64_t Get() const { return m_UUID; }

        bool operator==(const UUID& other) const
        {
            return m_UUID == other.Get();
        }

    private:
        uint64_t m_UUID = 0ull;
    };

    // 128 bit
    class UUID128
    {
    public:
        UUID128() = default;
        virtual ~UUID128() = default;

        static UUID128 Create();

        inline uint128_t Get() const { return m_UUID; }

    private:
        uint128_t m_UUID = 0ull;
    };

}

namespace std 
{
    template<>
    struct hash<Lavender::UUID> 
    {
        // Define the operator() to generate the hash
        uint64_t operator()(const Lavender::UUID& uuid) const 
        {
            return uuid.Get();
        }
    };
}