#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"7CA112211C73A956A80AF73358965BD2535FB579DF641504DA37D9BC92422DBC",
	};

	static std::string getAnticheatChecksum();

public:
	static bool IsRunningGoodKnownVersion();
};
