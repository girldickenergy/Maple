#include "MilkMemory.h"

#include <VirtualizerSDK.h>

MilkMemory::MilkMemory()
{
	VIRTUALIZER_FISH_RED_START
	_memoryRegions = std::vector<MemoryRegion>();
	cacheMemoryRegions();

	CODE_CAVE_SEARCH_OFFSET = reinterpret_cast<uintptr_t>(GetModuleHandleA("kernel32.dll")) - 0x10000000;
	VIRTUALIZER_FISH_RED_END
}

MilkMemory::~MilkMemory()
{
	VIRTUALIZER_FISH_RED_START
	_memoryRegions.clear();
	_memoryRegions.shrink_to_fit();
	VIRTUALIZER_FISH_RED_END
}

void MilkMemory::cacheMemoryRegions()
{
	VIRTUALIZER_LION_BLACK_START
	_memoryRegions.clear();

	MEMORY_BASIC_INFORMATION32 mbi{};
	LPVOID address = nullptr;
	bool currentRegionInvalid = false;
	while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
	{
		address = reinterpret_cast<LPVOID>(mbi.BaseAddress + mbi.RegionSize);
		if (mbi.BaseAddress == NULL)
			continue;

		// Search for only PE Headers in memory
		if (mbi.AllocationProtect == PAGE_EXECUTE_WRITECOPY &&
			mbi.State == MEM_COMMIT &&
			mbi.Protect == PAGE_READONLY &&
			mbi.Type == MEM_IMAGE &&
			mbi.RegionSize == 0x1000)
		{
			PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(mbi.BaseAddress);
			if (dosHeader->e_magic != 0x5a4d)
				continue;
			PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(mbi.BaseAddress + dosHeader->e_lfanew);

			// The current region has CFG enabled and therefore should not be checked for code caves.
			// We also check if the region is part of a COM module, if it is we also skip that
			if (ntHeaders->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF ||
				ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress != NULL)
				currentRegionInvalid = true;
		}

		if (currentRegionInvalid)
		{
			// If we've found empty space within loaded modules, reset status.
			if (mbi.State == MEM_FREE && mbi.Protect == PAGE_NOACCESS)
				currentRegionInvalid = false;
		}
		else
			if (mbi.Protect >= PAGE_READONLY && mbi.Protect <= PAGE_EXECUTE_WRITECOPY)
				_memoryRegions.emplace_back(mbi);
	}
	VIRTUALIZER_LION_BLACK_END
}

std::vector<uint8_t> MilkMemory::ReadMemory(uint32_t startAddress, SIZE_T size)
{
	VIRTUALIZER_LION_BLACK_START
	auto buffer = std::vector<uint8_t>(size);
	for(SIZE_T i = 0; i < size; i++)
	{
		const auto address = reinterpret_cast<uint8_t*>(startAddress + i);
		buffer[i] = *address;
	}

	VIRTUALIZER_LION_BLACK_END

	return buffer;
}

std::vector<MemoryRegion>* MilkMemory::GetMemoryRegions()
{
	VIRTUALIZER_LION_BLACK_START
	auto ret = &_memoryRegions;
	VIRTUALIZER_LION_BLACK_END

	return ret;
}

[[clang::optnone]] uint32_t* MilkMemory::FindCodeCave()
{
	VIRTUALIZER_LION_BLACK_START
	for(auto const& region : _memoryRegions)
	{
		if ((region.State & MEM_COMMIT) && (region.Protect & PAGE_EXECUTE_READ) &&
			(region.Type & MEM_IMAGE) && region.RegionSize >= CODE_CAVE_MINIMUM_REGIONSIZE &&
			!(region.AllocationProtect & PAGE_READONLY))
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

	VIRTUALIZER_LION_BLACK_END

	return nullptr;
}
