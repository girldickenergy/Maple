#pragma once

enum class OsuKeys : int
{
    None = 0,
    M1 = 1,
    M2 = 2,
    K1 = 4,
    K2 = 8,
    Smoke = 16
};

inline OsuKeys operator&(OsuKeys a, OsuKeys b)
{
    return static_cast<OsuKeys>(static_cast<int>(a) & static_cast<int>(b));
}

inline OsuKeys operator|(OsuKeys a, OsuKeys b)
{
    return static_cast<OsuKeys>(static_cast<int>(a) | static_cast<int>(b));
}