#pragma once
#include <Windows.h>
#include "Structs/MemoryRegion.h"
#include <vector>

class MilkMemory {
	std::vector<MemoryRegion> _memoryRegions;

	void cacheMemoryRegions();
public:
	MilkMemory();
	~MilkMemory();

	std::vector<MemoryRegion>* GetMemoryRegions();
};