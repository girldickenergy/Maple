#pragma once
#include <Windows.h>
#include "Structs/MemoryRegion.h"
#include <vector>

class MilkMemory {
	uint32_t CODE_CAVE_SEARCH_OFFSET = 0x50000000;
	uint32_t CODE_CAVE_MINIMUM_SIZE = 0x06;
	uint32_t CODE_CAVE_MINIMUM_REGIONSIZE = 0x100;
	std::vector<MemoryRegion> _memoryRegions;

	void cacheMemoryRegions();
public:
	MilkMemory();
	~MilkMemory();

	std::vector<uint8_t> ReadMemory(uint32_t startAddress, SIZE_T size);
	std::vector<MemoryRegion>* GetMemoryRegions();
	uint32_t* FindCodeCave();
};