#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"4c4bdd6b8e89626ebcf999b1e7b4b2aa"
	};
public:
	static std::string GetAnticheatChecksum();
	static std::vector<uint8_t> GetAnticheatBytes();
	static std::vector<uint8_t> GetGameBytes();
	static bool IsRunningGoodKnownVersion();
};
