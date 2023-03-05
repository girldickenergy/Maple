#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"284c1dc8a524e46fbe7a7cf846d3d9b1"
	};
	
	static std::string GetAnticheatChecksum();
	static std::vector<uint8_t> GetAnticheatBytes();
	static std::vector<uint8_t> GetGameBytes();

public:
	static bool IsRunningGoodKnownVersion();
};
