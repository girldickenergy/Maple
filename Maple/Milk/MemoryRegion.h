#pragma once
#include <Windows.h>

struct MemoryRegion
{
    DWORD BaseAddress;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
    DWORD AllocationProtect;

    explicit MemoryRegion(const MEMORY_BASIC_INFORMATION32& memoryBasicInformation)
    {
        BaseAddress = memoryBasicInformation.BaseAddress;
        RegionSize = memoryBasicInformation.RegionSize;
        State = memoryBasicInformation.State;
        Protect = memoryBasicInformation.Protect;
        Type = memoryBasicInformation.Type;
        AllocationProtect = memoryBasicInformation.AllocationProtect;
    }
};