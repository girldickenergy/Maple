#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"3A1C15F9C776E2EEDCB7428D1B8B18F4B2C81BC4DC0221AB08A841A9A5328146",
		"48AA6F7ABA42FB16BA602FAB70A70A223EC6E7D757B7C2627C7CE00CBBEFE05B"
	};

	static std::string getAnticheatChecksum();

public:
	static bool IsRunningGoodKnownVersion();
};
