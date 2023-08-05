#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
    static inline std::vector<std::string> goodKnownChecksums =
    {
		"42045c2fd5e358adad4b17848bcf884e"
    };
public:
    static std::string GetAnticheatChecksum();
    static std::vector<uint8_t> GetAnticheatBytes();
    static std::vector<uint8_t> GetGameBytes();
    static bool IsRunningGoodKnownVersion();
};
