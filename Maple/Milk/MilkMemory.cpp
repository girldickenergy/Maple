#include "MilkMemory.h"

MilkMemory::MilkMemory()
{
    m_MemoryRegions = std::vector<MemoryRegion>();
    CacheMemoryRegions();
}

void MilkMemory::CacheMemoryRegions()
{
    m_MemoryRegions.clear();

    auto memoryBasicInformation = MEMORY_BASIC_INFORMATION32();
    LPVOID address = nullptr;
    bool currentRegionInvalid = false;
    while (VirtualQuery(address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&memoryBasicInformation), sizeof memoryBasicInformation) != 0)
    {
        address = reinterpret_cast<LPVOID>(memoryBasicInformation.BaseAddress + memoryBasicInformation.RegionSize);
        if (memoryBasicInformation.BaseAddress == NULL)
            continue;

        // Search for only PE Headers in memory
        if (memoryBasicInformation.AllocationProtect == PAGE_EXECUTE_WRITECOPY && memoryBasicInformation.State == MEM_COMMIT &&
            memoryBasicInformation.Protect == PAGE_READONLY && memoryBasicInformation.Type == MEM_IMAGE &&
            memoryBasicInformation.RegionSize == 0x1000)
        {
            const auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(memoryBasicInformation.BaseAddress);
            if (dosHeader->e_magic != 0x5a4d)
                continue;

            // The current region has CFG enabled and therefore should not be checked for code caves.
            // We also check if the region is part of a COM module, if it is we also skip that
            if (const auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(memoryBasicInformation.BaseAddress + dosHeader->e_lfanew);
                ntHeaders->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF ||
                ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress != NULL)
                currentRegionInvalid = true;
        }

        if (currentRegionInvalid)
        {
            // If we've found empty space within loaded modules, reset status.
            if (memoryBasicInformation.State == MEM_FREE && memoryBasicInformation.Protect == PAGE_NOACCESS)
                currentRegionInvalid = false;
        }
        else if (memoryBasicInformation.Protect >= PAGE_READONLY && memoryBasicInformation.Protect <= PAGE_EXECUTE_WRITECOPY)
            m_MemoryRegions.emplace_back(memoryBasicInformation);
    }
}

std::vector<uint8_t> MilkMemory::ReadMemory(const uint32_t startAddress, const SIZE_T size)
{
    auto buffer = std::vector<uint8_t>(size);
    for (SIZE_T i = 0; i < size; i++)
    {
        const auto address = reinterpret_cast<uint8_t*>(startAddress + i);
        buffer[i] = *address;
    }

    return buffer;
}

std::vector<MemoryRegion>* MilkMemory::GetMemoryRegions()
{
    return &m_MemoryRegions;
}

uint32_t* MilkMemory::FindCodeCave()
{
    for (auto const& region : m_MemoryRegions)
    {
        if ((region.State & MEM_COMMIT) && (region.Protect & PAGE_EXECUTE_READ) && (region.Type & MEM_IMAGE) &&
            region.RegionSize >= m_CodeCaveMinimumRegionSize && !(region.AllocationProtect & PAGE_READONLY))
        {
            uint32_t readingAddress = region.BaseAddress;
            while (readingAddress < region.BaseAddress + region.RegionSize)
            {
                std::vector<uint8_t> memoryBuffer;
                memoryBuffer = ReadMemory(readingAddress, 1024);

                int occurrences = 0;
                for (size_t i = 0; i < memoryBuffer.size(); i++)
                {
                    auto const& byte = memoryBuffer[i];
                    if (byte == 0xCC)
                        occurrences++;
                    if (occurrences == m_CodeCaveMinimumSize)
                        return reinterpret_cast<uint32_t*>(readingAddress += i - (m_CodeCaveMinimumSize - 1));
                    if (occurrences > 0 && byte != 0xCC)
                        occurrences = 0;
                }

                readingAddress += memoryBuffer.size();
            }
        }
    }

    return nullptr;
}