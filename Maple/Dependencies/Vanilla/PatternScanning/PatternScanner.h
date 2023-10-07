#pragma once

#include <string>
#include <wtypes.h>

class PatternScanner
{
    unsigned long GetModuleSize(HMODULE module);
    uint8_t CharToByte(char ch);
    uint8_t StitchByte(char a, char b);

public:
    PatternScanner() = default;

    uintptr_t FindPatternInRange(const std::string& pattern, uintptr_t begin, unsigned int size);
    uintptr_t FindPatternInModule(const std::string& pattern, const std::string& moduleName);
    uintptr_t FindPattern(const std::string& pattern);
};