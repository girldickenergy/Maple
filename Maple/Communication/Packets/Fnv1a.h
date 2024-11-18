#include <cstdint>

constexpr uint32_t Val32Const = 0x811c9dc5;
constexpr uint32_t Prime32Const = 0x1000193;
constexpr uint64_t Val64Const = 0xcbf29ce484222325;
constexpr uint64_t Prime64Const = 0x100000001b3;

inline const uint32_t Hash32Fnv1a(const void* key, const uint32_t length)
{
    const char* data = reinterpret_cast<const char*>(key);
    uint32_t hash = Val32Const;

    for (size_t i = 0; i < length; i++)
    {
        uint8_t value = data[i];
        hash = hash ^ value;
        hash *= Prime32Const;
    }

    return hash;
}

inline const uint64_t Hash64Fnv1a(const void* key, const uint64_t length)
{
    const char* data = reinterpret_cast<const char*>(key);
    uint64_t hash = Val64Const;

    for (size_t i = 0; i < length; i++)
    {
        uint8_t value = data[i];
        hash = hash ^ value;
        hash *= Prime64Const;
    }

    return hash;
}

// A Fnv1a hasher that works on compile time
inline constexpr uint32_t Hash32Fnv1aConst(const char* str, const uint32_t value = Val32Const) noexcept
{
    return (str[0] == '\0') ? value : Hash32Fnv1aConst(&str[1], (value ^ uint32_t(static_cast<uint8_t>(str[0]))) * Prime32Const);
}

inline constexpr uint64_t Hash64Fnv1aConst(const char* str, const uint64_t value = Val64Const) noexcept
{
    return (str[0] == '\0') ? value : Hash64Fnv1aConst(&str[1], (value ^ uint64_t(static_cast<uint8_t>(str[0]))) * Prime64Const);
}