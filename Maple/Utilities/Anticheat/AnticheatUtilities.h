#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
    static inline std::vector<std::string> goodKnownChecksums =
    {
		"47183d7843a1af1e856bc2600ca73e5a"
    };
public:
    static std::string GetAnticheatChecksum();
    static std::vector<uint8_t> GetAnticheatBytes();
    static std::vector<uint8_t> GetGameBytes();
    static bool IsRunningGoodKnownVersion();
};
