#pragma once

#include <vector>

struct Patch
{
	uintptr_t Location = NULL;
	std::vector<char> OriginalBytes;

	Patch() = default;
	
	Patch(uintptr_t location, const std::vector<char>& originalBytes)
	{
		Location = location;
		OriginalBytes = originalBytes;
	}
};
