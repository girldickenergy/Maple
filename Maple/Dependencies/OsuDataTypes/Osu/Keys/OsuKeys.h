#pragma once

enum class OsuKeys : int
{
    None = 0,
    M1 = 1 << 0,
    M2 = 1 << 1,
    K1 = (1 << 2) + M1,
    K2 = (1 << 3) + M2,
    Smoke = 1 << 4,
};

inline OsuKeys operator&(OsuKeys a, OsuKeys b)
{
    return static_cast<OsuKeys>(static_cast<int>(a) & static_cast<int>(b));
}

inline OsuKeys operator|(OsuKeys a, OsuKeys b)
{
    return static_cast<OsuKeys>(static_cast<int>(a) | static_cast<int>(b));
}