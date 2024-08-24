#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
    static inline std::vector<std::string> goodKnownChecksums =
    {
		"341ad9b2d5806269ab112d0e8161670d"
    };
public:
    static std::string GetAnticheatChecksum();
    static std::vector<uint8_t> GetAnticheatBytes();
    static std::vector<uint8_t> GetGameBytes();
    static bool IsRunningGoodKnownVersion();
};
