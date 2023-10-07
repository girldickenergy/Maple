#pragma once

#include <string>
#include <vector>

struct Hook
{
    uintptr_t FunctionAddress;
    uintptr_t TrampolineAddress;
    std::vector<uint8_t> FunctionPrologue;

    Hook() = default;

    Hook(uintptr_t functionAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue)
    {
        FunctionAddress = functionAddress;
        TrampolineAddress = trampolineAddress;
        FunctionPrologue = functionPrologue;
    }
};