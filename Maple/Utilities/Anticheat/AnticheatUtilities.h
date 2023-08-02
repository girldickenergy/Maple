#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
    static inline std::vector<std::string> goodKnownChecksums =
    {
	"47a45f8a7e27136d6cb7a79bf369d1cc",
        "660088be2bc8b578cc6bed69020bbd19"
    };
public:
    static std::string GetAnticheatChecksum();
    static std::vector<uint8_t> GetAnticheatBytes();
    static std::vector<uint8_t> GetGameBytes();
    static bool IsRunningGoodKnownVersion();
};
