#pragma once
#include "MemoryRegion.h"
#include <Windows.h>
#include <vector>

class MilkMemory
{
    uint32_t m_CodeCaveSearchOffset = 0x50000000;
    uint32_t m_CodeCaveMinimumSize = 0x06;
    uint32_t m_CodeCaveMinimumRegionSize = 0x100;
    std::vector<MemoryRegion> m_MemoryRegions;

    void CacheMemoryRegions();

public:
    MilkMemory();

    std::vector<uint8_t> ReadMemory(uint32_t startAddress, SIZE_T size);
    std::vector<MemoryRegion>* GetMemoryRegions();
    uint32_t* FindCodeCave();
};