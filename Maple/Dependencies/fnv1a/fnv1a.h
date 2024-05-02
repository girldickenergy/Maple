#pragma once

#include <cstdint>
#include <cstring>

class fnv1a
{
    static inline uint32_t m_Prime = 0x01000193;
    static inline uint32_t m_Seed = 0x811C9DC5;

public:
    static uint32_t Hash(unsigned char byte, uint32_t hash = m_Seed)
    {
        return (byte ^ hash) * m_Prime;
    }

    static uint32_t Hash(const char* str, uint32_t hash = m_Seed)
    {
        size_t length = strlen(str);

        while (length--)
            hash = Hash(*reinterpret_cast<const unsigned char*>(str++), hash);

        return hash;
    }
};