#include "PatternScanner.h"

#include <vector>

unsigned long PatternScanner::GetModuleSize(HMODULE module)
{
    if (!module)
        return 0;

    const auto pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
    const auto pNTHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(pDOSHeader) + pDOSHeader->e_lfanew);

    return pNTHeaders->OptionalHeader.SizeOfImage;
}

uint8_t PatternScanner::CharToByte(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;

    return 0xCC;
}

uint8_t PatternScanner::StitchByte(char a, char b)
{
    return CharToByte(a) << 4 | CharToByte(b);
}

uintptr_t PatternScanner::FindPatternInRange(const std::string& pattern, uintptr_t begin, unsigned int size)
{
    std::vector<uint8_t> patternBytes;
    for (unsigned int i = 0u; i < pattern.size(); i++)
    {
        const char& ch = pattern[i];

        if (ch == '?')
        {
            if (i > 0 && pattern[i - 1] != '?')
                patternBytes.push_back(0xCC);
        }
        else if (std::isxdigit(ch))
            patternBytes.push_back(StitchByte(ch, pattern[++i]));
    }

    for (uintptr_t address = begin; address < begin + size - patternBytes.size(); address++)
    {
        bool found = true;

        for (unsigned int i = 0; i < patternBytes.size(); i++)
        {
            if (patternBytes[i] != 0xCC && patternBytes[i] != *reinterpret_cast<uint8_t*>(address + i))
            {
                found = false;

                break;
            }
        }

        if (found)
            return address;
    }

    return 0u;
}

uintptr_t PatternScanner::FindPatternInModule(const std::string& pattern, const std::string& moduleName)
{
    const HMODULE module = GetModuleHandleA(moduleName.c_str());

    return FindPatternInRange(pattern, reinterpret_cast<uintptr_t>(module), GetModuleSize(module));
}

uintptr_t PatternScanner::FindPattern(const std::string& pattern)
{
    MEMORY_BASIC_INFORMATION32 mbi;
    LPCVOID address = nullptr;

    while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
    {
        if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && !(mbi.Protect & PAGE_GUARD) && (mbi.Protect & PAGE_EXECUTE_READWRITE))
            if (const uintptr_t result = FindPatternInRange(pattern, mbi.BaseAddress, mbi.RegionSize))
                return result;

        address = reinterpret_cast<LPCVOID>(mbi.BaseAddress + mbi.RegionSize);
    }

    return 0u;
}
