#pragma once

#include <string>
#include <vector>

#include "EncryptedString.h"

class VanillaHook
{
public:
	EncryptedString Name;
	uintptr_t FunctionAddress;
	uintptr_t TrampolineAddress;
	std::vector<uint8_t> FunctionPrologue;

	VanillaHook(const char* name, uintptr_t functionAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue) : Name(name), FunctionAddress(functionAddress), TrampolineAddress(trampolineAddress), FunctionPrologue(functionPrologue) {}
};
