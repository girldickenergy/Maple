#include "VanillaPatternScanner.h"

#include <iostream>
#include <vector>

#include "Utilities/MemoryUtilities.h"

uint8_t VanillaPatternScanner::charToByte(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

	if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

	if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;

    return 0xCC;
}

uint8_t VanillaPatternScanner::stichByte(char a, char b)
{
    return charToByte(a) << 4 | charToByte(b);
}

uintptr_t VanillaPatternScanner::FindPatternInRange(const std::string& pattern, uintptr_t begin, unsigned int size, unsigned int offset, unsigned int readCount)
{
    std::vector<uint8_t> sig;
    for (unsigned int i = 0u; i < pattern.size(); i++)
	{
        const char& ch = pattern[i];
		
        if (ch == '?')
        {
            if (i > 0 && pattern[i - 1] != '?')
                sig.push_back(0xCC);
        }
        else if (std::isxdigit(ch))
            sig.push_back(stichByte(ch, pattern[++i]));
    }

    for (uintptr_t mem = begin; mem < begin + size - sig.size(); mem++)
    {
        bool found = true;
		
        for (unsigned int i = 0; i < sig.size(); i++)
        {
            if (sig[i] != 0xCC && sig[i] != *reinterpret_cast<uint8_t*>(mem + i))
            {
                found = false;

                break;
            }
        }

        if (found)
        {
            uintptr_t result = mem + offset;
	        for (unsigned int i = 0; i < readCount; i++)
				result = *reinterpret_cast<uintptr_t*>(result);
			
			return result;
        }
    }
	
    return 0u;
}

uintptr_t VanillaPatternScanner::FindPatternInModule(const std::string& pattern, const std::string& moduleName, unsigned int offset, unsigned int readCount)
{
    const HMODULE module = GetModuleHandleA(moduleName.c_str());

    return FindPatternInRange(pattern, reinterpret_cast<uintptr_t>(module), MemoryUtilities::GetModuleSize(module), offset, readCount);
}

uintptr_t VanillaPatternScanner::FindPattern(const std::string& pattern, unsigned int offset, unsigned int readCount)
{
    MEMORY_BASIC_INFORMATION32 mbi;
    LPCVOID address = nullptr;

    while (VirtualQueryEx(GetCurrentProcess(), address, reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&mbi), sizeof mbi) != 0)
    {
        if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS && !(mbi.Protect & PAGE_GUARD) && (mbi.Protect & PAGE_EXECUTE_READWRITE))
	        if (const uintptr_t result = FindPatternInRange(pattern, mbi.BaseAddress, mbi.RegionSize, offset, readCount))
                return result;

        address = reinterpret_cast<LPCVOID>(mbi.BaseAddress + mbi.RegionSize);
    }
	
    return 0u;
}
