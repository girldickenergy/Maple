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
	while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
	{
		if (mbi.Protect >= PAGE_READONLY && mbi.Protect <= PAGE_EXECUTE_WRITECOPY)
			_memoryRegions.emplace_back(mbi);

		address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
	}
}

std::vector<uint8_t> MilkMemory::ReadMemory(uint32_t startAddress, SIZE_T size)
{
	auto buffer = std::vector<uint8_t>(size);
	for(SIZE_T i = 0; i < size; i++)
	{
		const auto address = reinterpret_cast<uint8_t*>(startAddress + i);
		buffer[i] = *address;
	}

	return buffer;
}

std::vector<MemoryRegion>* MilkMemory::GetMemoryRegions()
{
	return &_memoryRegions;
}

uint32_t* MilkMemory::FindCodeCave()
{
	for(auto const& region : _memoryRegions)
	{
		if (region.State != MEM_FREE && region.Protect == PAGE_EXECUTE_READ &&
			region.Type == MEM_IMAGE && region.RegionSize >= CODE_CAVE_MINIMUM_REGIONSIZE &&
			region.AllocationProtect != PAGE_READONLY && region.BaseAddress >= CODE_CAVE_SEARCH_OFFSET)
		{
			uint32_t readingAddress = region.BaseAddress;
			while (readingAddress < region.BaseAddress + region.RegionSize)
			{
				auto memoryBuffer = ReadMemory(readingAddress, 1024);

				int occurrences = 0;
				for (size_t i = 0; i < memoryBuffer.size(); i++)
				{
					auto const& byte = memoryBuffer[i];
					if (byte == 0xCC)
						occurrences++;
					if (occurrences == CODE_CAVE_MINIMUM_SIZE)
						return reinterpret_cast<uint32_t*>(readingAddress += i - (CODE_CAVE_MINIMUM_SIZE - 1));
					if (occurrences > 0 && byte != 0xCC)
						occurrences = 0;
				}

				readingAddress += memoryBuffer.size();
			}
		}
	}

	return nullptr;
}