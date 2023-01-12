#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"7CA112211C73A956A80AF73358965BD2535FB579DF641504DA37D9BC92422DBC",
		"3A1C15F9C776E2EEDCB7428D1B8B18F4B2C81BC4DC0221AB08A841A9A5328146"
	};

	static std::string getAnticheatChecksum();

public:
	static bool IsRunningGoodKnownVersion();
};
