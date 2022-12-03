#include "MilkMemory.h"

MilkMemory::MilkMemory()
{
	_memoryRegions = std::vector<MemoryRegion>();
	cacheMemoryRegions();
}

MilkMemory::~MilkMemory()
{
	_memoryRegions.clear();
	_memoryRegions.shrink_to_fit();
}

void MilkMemory::cacheMemoryRegions()
{
	_memoryRegions.clear();

	MEMORY_BASIC_INFORMATION32 mbi{};
	LPVOID address = nullptr;
	while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
	{
		if (mbi.Protect >= 0x02 && mbi.Protect <= 0x80)
			_memoryRegions.emplace_back(mbi);

		address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
	}
}

std::vector<MemoryRegion>* MilkMemory::GetMemoryRegions()
{
	return &_memoryRegions;
}