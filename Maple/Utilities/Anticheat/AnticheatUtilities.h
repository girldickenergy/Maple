#pragma once

#include <string>
#include <vector>

class AnticheatUtilities
{
	static inline std::vector<std::string> goodKnownChecksums =
	{
		"CDC026B52B0CF1A35F65B06F64916ED01E9DBF202375D6DB94CDF6EB2B0C09E9",
		"C36A6B58635F7BC477AE0D45C3B2088DD2DC5F46A1C695018C9C469BFB395295"
	};

	static std::string getAnticheatChecksum();

public:
	static bool IsRunningGoodKnownVersion();
};
