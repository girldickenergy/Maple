#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"b05fdb385faca13a8ceba32167deada7"
	};
public:
	static std::string GetAnticheatChecksum();
	static std::vector<uint8_t> GetAnticheatBytes();
	static std::vector<uint8_t> GetGameBytes();
	static bool IsRunningGoodKnownVersion();
};
