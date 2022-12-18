#pragma once

#include <vector>
#include <wtypes.h>

class MemoryUtilities
{
public:
	static unsigned long GetModuleSize(HMODULE module);
	static std::vector<uint8_t> IntToByteArray(int value);
};