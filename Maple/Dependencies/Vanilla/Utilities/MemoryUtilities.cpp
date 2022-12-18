#include "MemoryUtilities.h"

unsigned long MemoryUtilities::GetModuleSize(HMODULE module)
{
	if (module == nullptr)
		return 0;

	IMAGE_DOS_HEADER* pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(module);
	IMAGE_NT_HEADERS* pNTHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(pDOSHeader) + pDOSHeader->e_lfanew);

	return pNTHeaders->OptionalHeader.SizeOfImage;
}

std::vector<uint8_t> MemoryUtilities::IntToByteArray(int value)
{
	std::vector<unsigned char> result(4);

	for (int i = 0; i < 4; i++)
		result[i] = (value >> (i * 8));

	return result;
}
