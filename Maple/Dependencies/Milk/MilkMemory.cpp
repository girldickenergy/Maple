#include "MilkMemory.h"

#include <ThemidaSDK.h>
#pragma optimize("", off)

MilkMemory::MilkMemory()
{
	VM_FISH_RED_START
	_memoryRegions = std::vector<MemoryRegion>();
	cacheMemoryRegions();
	VM_FISH_RED_END
}

MilkMemory::~MilkMemory()
{
	VM_FISH_RED_START
	_memoryRegions.clear();
	_memoryRegions.shrink_to_fit();
	VM_FISH_RED_END
}

void MilkMemory::cacheMemoryRegions()
{
	VM_LION_BLACK_START
	_memoryRegions.clear();

	MEMORY_BASIC_INFORMATION32 mbi{};
	LPVOID address = nullptr;
	while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
	{
		if (mbi.Protect >= PAGE_READONLY && mbi.Protect <= PAGE_EXECUTE_WRITECOPY)
			_memoryRegions.emplace_back(mbi);

		address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
	}
	VM_LION_BLACK_END
}

std::vector<uint8_t> MilkMemory::ReadMemory(uint32_t startAddress, SIZE_T size)
{
	VM_LION_BLACK_START
	auto buffer = std::vector<uint8_t>(size);
	for(SIZE_T i = 0; i < size; i++)
	{
		const auto address = reinterpret_cast<uint8_t*>(startAddress + i);
		buffer[i] = *address;
	}

	return buffer;
	VM_LION_BLACK_END
}

std::vector<MemoryRegion>* MilkMemory::GetMemoryRegions()
{
	VM_LION_BLACK_START
	return &_memoryRegions;
	VM_LION_BLACK_END
}

uint32_t* MilkMemory::FindCodeCave()
{
	VM_LION_BLACK_START
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
	VM_LION_BLACK_END
}