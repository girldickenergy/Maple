#pragma once

#include <string>

class VanillaPatternScanner
{
	static uint8_t charToByte(char ch);
	static uint8_t stichByte(char a, char b);
public:
	static uintptr_t FindPatternInRange(const std::string& pattern, uintptr_t begin, unsigned int size, unsigned int offset = 0, unsigned int readCount = 0, bool resolveRelativeAddress = false);
	static uintptr_t FindPatternInModule(const std::string& pattern, const std::string& moduleName, unsigned int offset = 0, unsigned int readCount = 0, bool resolveRelativeAddress = false);
	static uintptr_t FindPattern(const std::string& pattern, unsigned int offset = 0, unsigned int readCount = 0, bool resolveRelativeAddress = false);
	static uintptr_t FindPatternRW(const std::string& pattern, unsigned int offset = 0, unsigned int readCount = 0, bool resolveRelativeAddress = false);
};
