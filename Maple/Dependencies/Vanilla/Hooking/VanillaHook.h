#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include "VanillaHookType.h"

class VanillaHook
{
public:
	std::string Name;
	uintptr_t FunctionAddress;
	uintptr_t TrampolineAddress;
	std::vector<uint8_t> FunctionPrologue;
	VanillaHookType Type;

	VanillaHook(const std::string& name, uintptr_t functionAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue, VanillaHookType type) : Name(name), FunctionAddress(functionAddress), TrampolineAddress(trampolineAddress), FunctionPrologue(functionPrologue), Type(type) {}
};

class VanillaInlineHook : public VanillaHook
{
public:
	VanillaInlineHook(const std::string& name, uintptr_t functionAddress, uintptr_t trampolineAddress, const std::vector<uint8_t>& functionPrologue) : VanillaHook(name, functionAddress, trampolineAddress, functionPrologue, VanillaHookType::Inline) {}
};
class VanillaHardwareBreakpointHook : public VanillaHook
{
public:
	uintptr_t DetourFunctionAddress;
	HANDLE ThreadHandle;

	VanillaHardwareBreakpointHook(const std::string& name, uintptr_t functionAddress, uintptr_t detourFunctionAddress, uintptr_t codeCaveAddress, const std::vector<uint8_t>& prologue, HANDLE threadHandle) : VanillaHook(name, functionAddress, codeCaveAddress, prologue, VanillaHookType::HardwareBreakpoint)
	{
		DetourFunctionAddress = detourFunctionAddress;
		ThreadHandle = threadHandle;
	}
};