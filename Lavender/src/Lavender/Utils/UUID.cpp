#include "lvpch.h"
#include "UUID.hpp"

#include "Lavender/Core/Logging.hpp"

#include <random>

namespace Lavender
{

    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_RandomGenerator(s_RandomDevice());

    UUID UUID::Create()
    {
        UUID uuid = {};

        std::uniform_int_distribution<uint64_t> distribution(0ULL, std::numeric_limits<uint64_t>::max());

        uuid.m_UUID = distribution(s_RandomGenerator);

        return uuid;
    }

    UUID128 UUID128::Create()
    {
        UUID128 uuid = {};

        std::uniform_int_distribution<uint64_t> distribution(0ULL, std::numeric_limits<uint64_t>::max());

        uuid.m_UUID.Low = distribution(s_RandomGenerator);
        uuid.m_UUID.High = distribution(s_RandomGenerator);

        return uuid;
    }

}
