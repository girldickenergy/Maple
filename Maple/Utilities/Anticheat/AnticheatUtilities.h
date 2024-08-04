#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
    static inline std::vector<std::string> goodKnownChecksums =
    {
		"f9e1525b6d2c8fb6e44285deadd64004"
    };
public:
    static std::string GetAnticheatChecksum();
    static std::vector<uint8_t> GetAnticheatBytes();
    static std::vector<uint8_t> GetGameBytes();
    static bool IsRunningGoodKnownVersion();
};
