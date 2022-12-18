#pragma once

#include <vector>

struct VanillaPatch
{
	uintptr_t Address;
	std::vector<uint8_t> OriginalBytes;

	VanillaPatch(uintptr_t address, const std::vector<uint8_t>& originalBytes)
	{
		Address = address;
		OriginalBytes = originalBytes;
	}

	VanillaPatch() = default;
};
