#pragma once

#include <string>
#include <vector>

class VanillaHook
{
public:
	std::string Name;
	uintptr_t FunctionAddress;
	uintptr_t DetourStubAddress;
	uintptr_t TrampolineAddress;
	std::vector<uint8_t> FunctionPrologue;

	VanillaHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourStubAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue) : Name(name), FunctionAddress(functionAddress), DetourStubAddress(detourStubAddress), TrampolineAddress(trampolineAddress), FunctionPrologue(functionPrologue) {}
};